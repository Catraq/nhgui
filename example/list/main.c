#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "math/vec.h"
#include "nhgui_glfw.h"
#include "nhgui.h"

struct nhgui_object_scroll_bar 
{

	/* Amount of scroll in mm */
	float scroll_y_mm;
	float scroll_x_mm;	

	/* Used for the scroll bars */
	struct nhgui_icon_blank_object blank_scroll_y;
	struct nhgui_icon_blank_object blank_scroll_x;
};


struct nhgui_result
nhgui_object_scroll_bar_scroll_result(
		struct nhgui_object_scroll_bar *bar,
		struct nhgui_result result
)
{
	result.y_offset_mm += bar->scroll_y_mm;
	result.x_offset_mm -= bar->scroll_x_mm;
	return result;
}

void
nhgui_object_scroll_bar(
		struct nhgui_object_scroll_bar *bar,
		struct nhgui_context *context,
		struct nhgui_render_attribute *scroll_attribute,
		struct nhgui_render_attribute *size_attribute,
		struct nhgui_input *input, 
		struct nhgui_result scroll_result,	
		struct nhgui_result result	
)
{

	/* Compute the overflow in x and y direction and the % of overflow 
	 * compared to the size attribute corresponds to space that can be 
	 * scrolled.
	 *
	 * The y scroll bar consumed space in x direction so the x bar have 
	 * to be adjusted to be able to scroll past the space consumed by 
	 * the y scroll bar. 
	 * */
	
	float x_mm_used = 0.0f;
	float overflow_y_mm = size_attribute->height_mm - result.y_min_mm;
	if(overflow_y_mm > 0.0f)
	{	
		/* Compute % that is overflowing */
		float y_mm_size = size_attribute->height_mm;
		float p = (y_mm_size-overflow_y_mm)/(y_mm_size);
		
		/* Scale scroll bar with the %. If to small make it height */
		float scroll_bar_size_mm = y_mm_size * p < scroll_attribute->height_mm ? scroll_attribute->height_mm : y_mm_size * p;
		struct nhgui_render_attribute blank_scroll_attribute = {
			.height_mm = scroll_bar_size_mm,
			.width_mm = scroll_attribute->width_mm,
			.r = scroll_attribute->r,
			.g = scroll_attribute->g,
			.b = scroll_attribute->b
		};
		
		/* size conusmed by the y scroll bar */
		x_mm_used = blank_scroll_attribute.width_mm;
		
		/* Place the scrollbar in the right of the scroll result */	
		struct nhgui_result scroll_result_y = scroll_result;
		scroll_result_y.x_mm = scroll_result.x_mm + size_attribute->width_mm - blank_scroll_attribute.width_mm;
		scroll_result_y.y_mm -= bar->scroll_y_mm;


		
		nhgui_icon_blank(
				context, 
				&bar->blank_scroll_y,
				&blank_scroll_attribute,
				input, 
				scroll_result_y
		);

		if(bar->blank_scroll_y.pressed > 0)
		{
			/* Allow moving the scroll bar */	
			float scroll_area_mm = y_mm_size - scroll_bar_size_mm;
			float mm_per_pixel = (float)context->height_mm/(float)context->res_y;
			float scroll_in_mm = input->cursor_y_delta * mm_per_pixel;
			float max_scroll = bar->scroll_y_mm + scroll_in_mm < scroll_area_mm ? scroll_in_mm : bar->scroll_y_mm + scroll_in_mm - scroll_area_mm;
			bar->scroll_y_mm = bar->scroll_y_mm-max_scroll > 0 ? bar->scroll_y_mm-max_scroll : 0;


		
		}
	}

	float x_mm_size = size_attribute->width_mm;
	float x_overflow_mm = result.x_max_mm + 2*x_mm_used - x_mm_size - scroll_result.x_mm;
	if(x_overflow_mm > 0.0)
	{
		float p = (x_mm_size-x_overflow_mm)/(x_mm_size);

		float scroll_bar_size_mm = x_mm_size * p < scroll_attribute->height_mm ? scroll_attribute->height_mm : x_mm_size * p;
		struct nhgui_render_attribute blank_scroll_attribute = {
			.height_mm = scroll_attribute->width_mm,
			.width_mm = scroll_bar_size_mm,
			.r = scroll_attribute->r,
			.g = scroll_attribute->g,
			.b = scroll_attribute->b
		};


		struct nhgui_result scroll_result_x = {
			.x_mm = scroll_result.x_mm + bar->scroll_x_mm,
			.y_mm = scroll_result.y_mm + scroll_attribute->width_mm - size_attribute->height_mm,
		};

		
		nhgui_icon_blank(
				context, 
				&bar->blank_scroll_x,
				&blank_scroll_attribute,
				input, 
				scroll_result_x	
		);
		if(bar->blank_scroll_x.pressed > 0)
		{
			float scroll_area_mm = x_mm_size - scroll_bar_size_mm - x_mm_used;
			float mm_per_pixel = (float)context->width_mm/(float)context->res_x;
			float scroll_in_mm = input->cursor_x_delta * mm_per_pixel;
			float max_scroll = bar->scroll_x_mm + scroll_in_mm < 0 ? 0 : scroll_in_mm;
			bar->scroll_x_mm = bar->scroll_x_mm+max_scroll < scroll_area_mm ? bar->scroll_x_mm+max_scroll : scroll_area_mm;


		
		}

	
	
	}


}

struct nhgui_window 
{
	/* Attributes of the scroll bar. Shall be set. */
	struct nhgui_render_attribute scroll_bar_attribute;

	/* Internals */
	struct nhgui_result result_begin;
	struct nhgui_object_scroll_bar scroll_bar;
};


struct nhgui_result
nhgui_window_begin(
		struct nhgui_window *window,
		struct nhgui_context *context,
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input,
		struct nhgui_result result
)
{
	/* Store the result such that the window initial positions are known */
	window->result_begin = result;
	
	/* Compute the screen coordinates of the window and only alllow drawing 
	 * to the pixels within the area using scissor test in opengl. */	
	float x_pixels_per_mm = (float)context->res_x/(float)context->width_mm;
	float y_pixels_per_mm = (float)context->res_y/(float)context->height_mm;
	
	uint32_t x = result.x_mm * x_pixels_per_mm;
	uint32_t y = (result.y_mm - attribute->height_mm)*y_pixels_per_mm < 0 ? 0 : (result.y_mm - attribute->height_mm)*y_pixels_per_mm; 
	uint32_t height_remove_mm = result.y_mm - attribute->height_mm < 0 ? -(result.y_mm - attribute->height_mm) : 0;
	uint32_t width = attribute->width_mm * x_pixels_per_mm;
	uint32_t height = (attribute->height_mm - height_remove_mm) * y_pixels_per_mm;

	glScissor(
		x,
		y,
		width,
		height	
	);


	/* Apply offsets that are adjusted by the scroll bars to the result. */
	result = nhgui_object_scroll_bar_scroll_result(
			&window->scroll_bar,
			result
	);

	return result;

}

struct nhgui_result 
nhgui_window_end(
		struct nhgui_window *window,
		struct nhgui_context *context,
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input,
		struct nhgui_result result
)
{
	
	struct nhgui_result window_result = window->result_begin;

	window_result.x_inc_next = attribute->width_mm;
	window_result.y_inc_next = attribute->height_mm;

	window_result.y_min_mm = result.y_min_mm < result.y_mm + result.y_inc_next ? result.y_min_mm : result.y_mm + result.y_inc_next;
	window_result.x_max_mm = result.x_max_mm < result.x_mm + result.x_inc_next ? result.x_mm + result.x_inc_next : result.x_max_mm;

	window_result.x_offset_mm = 0;
	window_result.y_offset_mm = 0;

	nhgui_object_scroll_bar(
			&window->scroll_bar,
			context,
			&window->scroll_bar_attribute, 
			attribute,
			input,
			window->result_begin,
			result
	);


	return window_result;

}
			



struct nhgui_object_font_text_area 
{
	struct vec3 background_color;	

	struct vec3 font_color;
};

struct nhgui_result 
nhgui_object_font_text_area(
		struct nhgui_context *context,
		struct nhgui_object_font_text_area *area,
		struct nhgui_object_font *font,
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result,
		const char *input_buffer, 
		uint32_t input_buffer_size
)

{
	struct nhgui_result background_result = result; 
	uint32_t curr_char = 0;

	while(curr_char < input_buffer_size)
	{
			
		struct nhgui_render_attribute blank_attribute = 
		{
			.width_mm = attribute->width_mm,
			.height_mm = attribute->height_mm,	
			.r = area->background_color.x,
			.g = area->background_color.y,
			.b = area->background_color.z,
		};

		/* Background of the input field. */
		background_result = nhgui_icon_blank_no_object(
				context,
				&blank_attribute,
				input,
				background_result	
		);


		
		float x_mm = 0.0f;
		float x_mm_max = background_result.x_mm + background_result.x_inc_next;

		/* Make sure that curr_char always is increased by one */
		uint32_t char_within = 1;
		for(uint32_t i = curr_char; i < input_buffer_size; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->width_mm/(float)context->res_x;
			
			float new_x_mm = x_mm + (float)(font->character[c].advance_x >> 6) * mm_per_pixel_x;
			
		

			/* See if it is past the boudning box */
			if(background_result.x_mm + new_x_mm > x_mm_max)
			{
				break;
			}

			char_within += 1;

			x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
		}

		/* Remove the character that was there just in case */
		if(char_within > 1){
			char_within -= 1;	
		}
		
	
		struct nhgui_render_attribute font_attribute = 
		{
			.height_mm = attribute->height_mm,	
			.r = area->font_color.x,
			.g = area->font_color.y,
			.b = area->font_color.z,
		};


		nhgui_object_font_text(
			context, 
			font, 
			&input_buffer[curr_char],
			char_within,
			&font_attribute,
			input, 
			background_result
		);

		curr_char += char_within;

		background_result = nhgui_result_dec_y(background_result);
	
	}

	return background_result;
}

#define ADD_BUFFER_SIZE 50
#define ADD_TEXT_COUNT 10

struct input_list_example 
{
	/* Add input field */
	uint32_t add_buffer_length;
	char add_buffer[ADD_BUFFER_SIZE];
	
	/* List with elements */	
	uint32_t add_text_used;
	char *add_text_buffer_ptr[ADD_TEXT_COUNT];

	uint8_t add_text_buffer[ADD_TEXT_COUNT][ADD_BUFFER_SIZE];
	uint32_t add_text_buffer_length[ADD_TEXT_COUNT];

	struct nhgui_icon_blank_object add_blank;
	struct nhgui_icon_blank_object delete_blank;

	struct nhgui_object_input_field add_field;
	struct nhgui_object_text_list list_object;
	struct nhgui_object_font_text_area text_area;

	struct nhgui_window nhwindow;
};

void 
input_list_example_initialize(
		struct input_list_example *example
)
{
	example->add_field = (struct nhgui_object_input_field){
		.field_color = {.x = 0.3, .y = 0.3, .z = 0.3},
	};

	example->list_object = (struct nhgui_object_text_list){
		.char_scroll_per_sec = 1,
		.text_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
		.field_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_text_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_field_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
	
	};
	
	example->text_area = (struct nhgui_object_font_text_area){
		.background_color = {.x = 0, .y = 0.3, .z = 0.3},
		.font_color = {.x=1.0, .y=1.0, .z=1.0},
	};

	
	example->nhwindow.scroll_bar_attribute = (struct nhgui_render_attribute){
		.width_mm = 3,
		.height_mm = 10,
		.r = 1.0
	};


}

struct nhgui_result
input_list_example(
		struct input_list_example *example,
		struct nhgui_context *context,
		struct nhgui_input *input,	
		struct nhgui_object_font *font,
		struct nhgui_result result
		)
{
	
	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
	};


	struct nhgui_render_attribute window_attribute = 
	{
		.width_mm = 50,
		.height_mm = 50,
	};

	
	result = nhgui_window_begin(
			&example->nhwindow,
			context,
			&window_attribute,
			input,
			result	
	);


	
	struct nhgui_result input_res = nhgui_result_margin(result, 1, 1);
	
	struct nhgui_render_attribute input_field_attribute = {
		.height_mm = 3,
		.width_mm = 30,			
	};

	struct nhgui_result res = nhgui_object_input_field(
			context,
			&example->add_field, 
			font,
			&input_field_attribute ,
			input, 
			input_res,
			example->add_buffer, 
			&example->add_buffer_length,
			ADD_BUFFER_SIZE
	);
	res = nhgui_result_inc_x(res);	

	struct nhgui_render_attribute blank_attribute = {
		.height_mm = res.y_inc_next,
		.width_mm = 10,	
		.r = 1.0,
	};
	
	const char add_text[] = "Add";

	res = nhgui_icon_blank(
		context,
		&example->add_blank,
		&blank_attribute,
		input,
		res
	);

	struct nhgui_result icon_blank_center = nhgui_object_font_text_result_centered_by_previous_x(
			res, 
			context,
			font,
			&radio_render_attribute,
			add_text,
			sizeof(add_text)
	);


	nhgui_object_font_text(
			context,
			font,
			add_text,
			sizeof(add_text),
			&radio_render_attribute,
			input, 
			icon_blank_center
	);



	if(example->add_blank.clicked > 0)
	{
		if(example->add_text_used < ADD_TEXT_COUNT)
		{
			example->add_text_used++;

			uint32_t free_index = 0;
			for(uint32_t i = 0 ; i < ADD_TEXT_COUNT; i++){
				if(example->add_text_buffer_ptr[i] == NULL){
					free_index = i;
					break;
				}
			}


			memcpy(example->add_text_buffer[free_index], example->add_buffer, ADD_BUFFER_SIZE);	
			example->add_text_buffer_length[free_index] = example->add_buffer_length;
			example->add_text_buffer_ptr[free_index] = &example->add_text_buffer[free_index];

			example->add_buffer_length = 0;
		}

	
	}






	res = nhgui_result_dec_y(res);	
	res = nhgui_result_rewind_x_to(res, input_res);

	struct nhgui_render_attribute input_list_attribute = {
		.height_mm = 3,
		.width_mm = 70,			
	};

	res = nhgui_object_text_list(
			context, 
			&example->list_object,
			example->add_text_buffer_ptr,
			example->add_text_buffer_length,
			ADD_TEXT_COUNT, 
			font, 
			&input_list_attribute,
			input, 
			res
	);
	

	if(example->add_text_used > 0)
	{	if(example->list_object.selected > 0)
		{	
			const char delete_text[] = "Delete";
			
			res = nhgui_result_inc_x(example->list_object.selected_result);

			res = nhgui_icon_blank(
				context,
				&example->delete_blank,
				&blank_attribute,
				input,
				res	
			);

			struct nhgui_result icon_blank_delete_center = nhgui_object_font_text_result_centered_by_previous_x(
					res, 
					context,
					font,
					&radio_render_attribute,
					delete_text,
					sizeof(delete_text)
			);


			nhgui_object_font_text(
					context,
					font,
					delete_text,
					sizeof(delete_text),
					&radio_render_attribute,
					input, 
					icon_blank_delete_center
			);

			if(example->delete_blank.clicked > 0)
			{

				uint32_t index = example->list_object.selected_index;
				example->add_text_buffer_ptr[index] = NULL;
				example->add_text_buffer_length[index] = 0;

				if(example->add_text_used != 0)
					example->add_text_used--;

				example->list_object.selected = 0;

			}
		}
	}
	
	res = nhgui_result_dec_y(res);

	const char text_area_text[] = { 
		"The input field will scroll when overfilled and the list will have "
		"scrolling text when overfilled. Try to overfill them. It is also "
		"possible to select text in the input field and edit it. If the "
		"entire window is overfilled can a scroll bar be used. "
	};

	struct nhgui_render_attribute text_area_attribute = 
	{
		.width_mm = 30,
		.height_mm = 3,	
	};

	
	res = nhgui_object_font_text_area(
			context,
			&example->text_area,
			font,
			&text_area_attribute,
			input, 
			res,
			text_area_text, 
			sizeof(text_area_text)
	);
	

	return nhgui_window_end(
			&example->nhwindow,
			context,
			&window_attribute,
			input,
			res	
	);


}



int main(int args, char *argv[])
{

	/* First initialize opengl context */
	GLFWwindow* window = 0;
		
	/* Initialize openGL */	
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint( GLFW_SAMPLES, 16 );

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		fprintf(stderr,"Failed to init GLFW \n");
		glfwTerminate();
		return -1;
	}
	
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

	int width_mm, height_mm;
	glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &width_mm, &height_mm);

	int res_x, res_y;
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	res_x = mode->width;
	res_y = mode->height;


	glfwSetCharCallback(window, nhgui_glfw_char_callback);

	glfwMakeContextCurrent(window);
	
	/* Disable vertical sync */
	//glfwSwapInterval(0);

  	
       	/* Enable Version 3.3 */
	glewExperimental = 1;
	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr,"Failed to init Glew \n");
		glfwTerminate();
		return -1;
	}


	/* Clear any errors */
	while( glGetError() != GL_NONE)
	{
		glGetError();
	}
	
	glfwSwapBuffers(window);
	glfwPollEvents();

	
	
	int result = 0;
	/* Create gui context */	
	struct nhgui_context context;
	result = nhgui_context_initialize(&context, (uint32_t)res_x, (uint32_t)res_y, (uint32_t)width_mm, (uint32_t)height_mm);
	if(result < 0){
		fprintf(stderr, "nhgui_context_initialize() failed. \n");
		exit(EXIT_FAILURE);
	}

	struct nhgui_render_attribute font_render_attribute = {
		.height_mm = 10,
	};


	


	const char *font_filename = "../data/UbuntuMono-R.ttf";
	struct nhgui_object_font font;

	{
		struct nhgui_object_font_freetype font_freetype;
		result = nhgui_object_font_freetype_initialize(&font_freetype);
		if(result < 0){
			fprintf(stderr, "nhgui_object_font_freetype_initilaize() failed. \n");
			exit(EXIT_FAILURE);
		}
		
		result = nhgui_object_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&font_render_attribute ,
				&font, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_object_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}
	

		nhgui_object_font_freetype_deinitialize(&font_freetype);
	}

	




	struct input_list_example example_1 = {};
	struct input_list_example example_2 = {};
	struct input_list_example example_3 = {};
	struct input_list_example example_4 = {};

	input_list_example_initialize(&example_1);
	input_list_example_initialize(&example_2);
	input_list_example_initialize(&example_3);
	input_list_example_initialize(&example_4);

	struct nhgui_glfw_frame frame = nhgui_frame_create(window);


	while(!glfwWindowShouldClose(window))
	{

		struct nhgui_input input = nhgui_glfw_frame_begin(&frame, window);

		glDisable(GL_SCISSOR_TEST);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(
			0, 
			0, 
			input.width,
			input.height
		);
		
		glEnable(GL_SCISSOR_TEST);

		struct nhgui_result result = {
			.y_mm = context.height_mm * (float)input.height/(float)context.res_y
		};
		
		/* Top left */
	       	struct nhgui_result result_top_left = input_list_example(
			&example_1,
			&context,
			&input,	
			&font,
			result
		);

		/* Bottom left */
		result = nhgui_result_dec_y(result_top_left);
		result = input_list_example(
			&example_2,
			&context,
			&input,	
			&font,
			result
		);

		/* Top right */
		result = nhgui_result_inc_x(result_top_left);
		result = input_list_example(
			&example_3,
			&context,
			&input,	
			&font,
			result
		);

		/* Top right */
		result = nhgui_result_dec_y(result);
		result = input_list_example(
			&example_4,
			&context,
			&input,	
			&font,
			result
		);








		nhgui_glfw_frame_end(&frame, &input);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
