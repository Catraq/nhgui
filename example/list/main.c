#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "math/vec.h"
#include "nhgui_glfw.h"
#include "nhgui.h"


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

	struct nhgui_icon_blank add_blank;
	struct nhgui_icon_blank delete_blank;

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
		.width_mm = 80,
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
			&example->add_field, 
			context,
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
		&example->add_blank,
		context,
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

	struct nhgui_result text_list_result = nhgui_object_text_list(
			&example->list_object,
			context, 
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
				&example->delete_blank,
				context,
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
	text_list_result.x_max_mm = text_list_result.x_max_mm < res.x_max_mm ? res.x_max_mm : text_list_result.x_max_mm;
	text_list_result.y_min_mm = text_list_result.y_min_mm < res.y_min_mm ? text_list_result.y_min_mm : res.y_min_mm; 
	

	res = nhgui_result_dec_y(text_list_result);

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
			&example->text_area,
			context,
			font,
			&text_area_attribute,
			input, 
			res,
			text_area_text, 
			sizeof(text_area_text)
	);

	res = nhgui_result_dec_y(res);
	

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
			.y_mm = context.height_mm * (float)input.height/(float)context.res_y,
			.y_min_mm = context.height_mm * (float)input.height/(float)context.res_y
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
