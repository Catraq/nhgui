#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "math/vec.h"
#include "nhgui_glfw.h"
#include "nhgui.h"

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

	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
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

	





	/* Add input field */
	const uint32_t add_buffer_size = 50;
	uint32_t add_buffer_length = 0;
	char add_buffer[add_buffer_size];
	
	/* List with elements */	
	uint32_t add_text_count = 10;	
	uint32_t add_text_used = 0;
	char *add_text_buffer_ptr[add_text_count];
	memset(add_text_buffer_ptr, 0, sizeof add_text_buffer_ptr);

	uint8_t add_text_buffer[add_text_count][add_buffer_size];
	uint32_t add_text_buffer_length[add_text_count];
	memset(add_text_buffer_length, 0, sizeof(add_text_buffer_length));


	struct nhgui_object_input_field add_field = {
		.field_color = {.x = 0.3, .y = 0.3, .z = 0.3},
	};

	struct nhgui_object_text_list list_object  = 
	{
		.char_scroll_per_sec = 1,
		.text_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
		.field_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_text_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_field_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
	
	};
	
	struct nhgui_object_font_text_area text_area = {
		.background_color = {.x = 0, .y = 0.3, .z = 0.3},
		.font_color = {.x=1.0, .y=1.0, .z=1.0},
	};

	struct nhgui_icon_blank_object add_blank;
	struct nhgui_icon_blank_object delete_blank;

	struct nhgui_glfw_frame frame = nhgui_frame_create();

	while(!glfwWindowShouldClose(window))
	{

		struct nhgui_input input = nhgui_glfw_frame_begin(&frame, window);

		glViewport(0, 0, input.width, input.height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		struct nhgui_result render_result_start = {};
		render_result_start.x_mm = 0;
		render_result_start.y_mm = context.height_mm * (float)input.height/(float)context.res_y;
		
		
		struct nhgui_result input_res = nhgui_result_margin(render_result_start, 1, 1);
		
		struct nhgui_render_attribute input_field_attribute = {
			.height_mm = 3,
			.width_mm = 30,			
		};

		struct nhgui_result res = nhgui_object_input_field(
				&context,
				&add_field, 
				&font,
				&input_field_attribute ,
				&input, 
				input_res,
				add_buffer, 
				&add_buffer_length,
				add_buffer_size
		);
		res = nhgui_result_inc_x(res);	

		struct nhgui_render_attribute blank_attribute = {
			.height_mm = res.y_inc_next,
			.width_mm = 10,	
			.r = 1.0,
		};
		
		const char add_text[] = "Add";

		res = nhgui_icon_blank(
			&context,
			&add_blank,
			&blank_attribute,
			&input,
			res
		);

		struct nhgui_result icon_blank_center = nhgui_object_font_text_result_centered_by_previous_x(
				res, 
				&context,
				&font,
				&radio_render_attribute,
				add_text,
				sizeof(add_text)
		);


		nhgui_object_font_text(
				&context,
				&font,
				add_text,
				sizeof(add_text),
				&radio_render_attribute,
				&input, 
				icon_blank_center
		);

	

		if(add_blank.clicked > 0)
		{
			if(add_text_used < add_text_count)
			{
				add_text_used++;

				uint32_t free_index = 0;
				for(uint32_t i = 0 ; i < add_text_count; i++){
					if(add_text_buffer_ptr[i] == NULL){
						free_index = i;
						break;
					}
				}


				memcpy(add_text_buffer[free_index], add_buffer, add_buffer_size);	
				add_text_buffer_length[free_index] = add_buffer_length;
				add_text_buffer_ptr[free_index] = &add_text_buffer[free_index];

				add_buffer_length = 0;
			}

		
		}






		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, input_res);
	
		struct nhgui_render_attribute input_list_attribute = {
			.height_mm = 3,
			.width_mm = 70,			
		};

		res = nhgui_object_text_list(
				&context, 
				&list_object,
				add_text_buffer_ptr,
				add_text_buffer_length,
				add_text_count, 
				&font, 
				&input_list_attribute,
				&input, 
				res
		);
		

		if(add_text_used > 0)
		{	if(list_object.selected > 0)
			{	
				const char delete_text[] = "Delete";
				
				res = nhgui_result_inc_x(list_object.selected_result);

				res = nhgui_icon_blank(
					&context,
					&delete_blank,
					&blank_attribute,
					&input,
					res	
				);

				struct nhgui_result icon_blank_delete_center = nhgui_object_font_text_result_centered_by_previous_x(
						res, 
						&context,
						&font,
						&radio_render_attribute,
						delete_text,
						sizeof(delete_text)
				);


				nhgui_object_font_text(
						&context,
						&font,
						delete_text,
						sizeof(delete_text),
						&radio_render_attribute,
						&input, 
						icon_blank_delete_center
				);

				if(delete_blank.clicked > 0)
				{

					uint32_t index = list_object.selected_index;
					add_text_buffer_ptr[index] = NULL;
					add_text_buffer_length[index] = 0;

					if(add_text_used != 0)
						add_text_used--;

					list_object.selected = 0;

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
				&context,
				&text_area,
				&font,
				&text_area_attribute,
				&input, 
				res,
				text_area_text, 
				sizeof(text_area_text)
		);

		nhgui_glfw_frame_end(&frame, &input);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
