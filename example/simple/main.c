#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "math/vec.h"
#include "nhgui_glfw.h"
#include "nhgui.h"


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

	
	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
		.width_mm = 40, 
	};



	const uint32_t radio_button_row = 5;
	struct 	nhgui_object_radio_button radio_button_object[radio_button_row];



	const char *radio_button_text[] = {
		"I accept that you may have my soul.",
		"I agree that the terms of service is good. ",
		"I hope this gui will be useful", 
		"I accept that I lack imagination",
		"pI may not have hope and dreams."
	};

	struct nhgui_window nhwindow = {};


	struct nhgui_render_attribute menu_render_attribute = {
		.height_mm = 10,
	};
	struct nhgui_icon_menu menu_object = {};

	/* Search input field */
	const uint32_t input_buffer_size = 32;
	uint32_t input_buffer_length = 0;
	char input_buffer[input_buffer_size];
	struct nhgui_object_input_field input_field = {};

	/* Add input field */
	const uint32_t add_buffer_size = 32;
	uint32_t add_buffer_length = 0;
	char add_buffer[input_buffer_size];
	struct nhgui_object_input_field add_field = {};

	struct nhgui_object_text_list list_object = {
		.text_color = (struct nhgui_vec3){.x = 1, .y = 0, .z = 0},
		.selected_text_color = (struct nhgui_vec3){.x = 0, .y = 1, .z = 0},
		.char_scroll_per_sec = 1.0,

	};
	uint32_t list_entries_length[] = 
	{
		strlen(radio_button_text[0]), 	
		strlen(radio_button_text[1]), 	
		strlen(radio_button_text[2]), 	
		strlen(radio_button_text[3]), 	
		strlen(radio_button_text[4]), 	
	};

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
			input.width_pixel,
			input.height_pixel
		);
		
		glEnable(GL_SCISSOR_TEST);

		struct nhgui_result result = {
			.y_mm = context.screen_height_mm * (float)input.height_pixel/(float)context.screen_resolution_y,
			.y_min_mm = context.screen_height_mm * (float)input.height_pixel/(float)context.screen_resolution_y
		};

		struct nhgui_render_attribute window_attribute = 
		{
			.width_mm = context.screen_width_mm * (float)input.width_pixel/(float)context.screen_resolution_x,
			.height_mm = context.screen_height_mm * (float)input.height_pixel/(float)context.screen_resolution_y
		};


		struct nhgui_result root_result = nhgui_window_begin(
			&nhwindow,
			&context,
			&window_attribute,
			&input,
			result	
		);

	

		/* Menu button */
		result = nhgui_result_margin(result, 1, 1);
		struct nhgui_result m_render_result = nhgui_icon_menu(
				&menu_object,
				&context,
			       	&menu_render_attribute,
			       	&input,
			       	result
		);

		result = nhgui_result_dec_y(m_render_result);

		/* Menu button text */
		const char menu_text[] = "menu";
		result = nhgui_result_margin(result, 0, 1);
		struct nhgui_result c_render_result = nhgui_object_font_text_result_centered_by_previous_x(
				result, 
				&context, 
				&font, 
				&radio_render_attribute,
				menu_text,
				sizeof(menu_text)
		);

		result = nhgui_object_font_text(
				&context, 
				&font, 
				menu_text,
				sizeof(menu_text),
				&radio_render_attribute,
				&input, 
				c_render_result
		);
		result = nhgui_result_dec_y(result);
		result = nhgui_result_margin(result, 0, 2);

	
		if(menu_object.clicked)
		{
			for(uint32_t j = 0; j < radio_button_row; j++)
			{

				result = nhgui_result_margin(result, 0, 0);

				uint32_t index = j;
				result = nhgui_object_radio_button(
						&radio_button_object[index], 
						&context,
						&radio_render_attribute, 
						&input,
					       	result
				);
				
				result = nhgui_result_inc_x(result);
				result = nhgui_result_margin(result, 2, 0);

				result = nhgui_object_font_text(
						&context, 
						&font, 
						radio_button_text[j], 
						strlen(radio_button_text[j]), 
						&radio_render_attribute,
						&input, 
						result
				);

				
				result = nhgui_result_margin(result, 0, 1);

				result = nhgui_result_rewind_x_to(result, c_render_result);
				result = nhgui_result_dec_y(result);

			}
					
		}
		
		result = nhgui_result_dec_y(result);
		result = nhgui_result_rewind_x_to(result, m_render_result);

		struct nhgui_result res = nhgui_object_input_field(
				&input_field, 
				&context,
				&font, 
				&radio_render_attribute,
				&input, 
				result,
				input_buffer, 
				&input_buffer_length,
				input_buffer_size
		
		);

		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, m_render_result); 
		res = nhgui_result_margin(res, 0, 1);	
			
		res = nhgui_object_input_field(
				&add_field, 
				&context,
				&font, 
				&radio_render_attribute,
				&input, 
				res,
				add_buffer, 
				&add_buffer_length,
				add_buffer_size
		);

		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, m_render_result); 


		res = nhgui_object_text_list(
				&list_object,
				&context, 
				radio_button_text,
				list_entries_length,
				5, 
				&font, 
				&radio_render_attribute,
				&input, 
				res
		);


	


		nhgui_window_end(
				&nhwindow,
				&context,
				&window_attribute,
				&input,
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
