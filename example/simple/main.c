#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "nhgui_glfw.h"
#include "nhgui.h"


struct nhgui_object_text_list_object
{
	uint32_t selected;
	uint32_t selecteed_index;
};


struct nhgui_result
nhgui_object_text_list(
		struct nhgui_context *context,
		struct nhgui_object_text_list_object *object,
		char *entry[],
		uint32_t *entry_length,
		uint32_t entry_count,
		struct nhgui_object_font_character character[128], 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result

)
{
	
	struct nhgui_render_attribute font_attribute = 
	{
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};

	for(uint32_t i = 0; i < entry_count; i++)
	{
#if 0
		float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
		float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;
	
		struct nhgui_result result_tmp = result;
		result_tmp.y_mm -= attribute->height_mm;
		
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			field->selected = field->selected ? 0 : 1;
			field->selected_prev = field->selected;

			if(field->selected > 0){
				input->selected_new = 1;	
			}
		}	
#endif 

		nhgui_icon_blank(
				context,
				attribute,
				input,
				result
		);


		result = nhgui_object_font_text(
				context, 
				character, 
				entry[i],
				entry_length[i],
				&font_attribute,
				input, 
				result
		);

		result = nhgui_result_dec_y(result);

	}

	return result;
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
		.width_mm = 40, 
	};



	const char *font_filename = "../data/UbuntuMono-R.ttf";
	struct nhgui_object_font_character character[128];
	struct nhgui_object_font_character radio_character[128];

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
				character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_object_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}
	
		result = nhgui_object_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&radio_render_attribute,
				radio_character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_object_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}


		nhgui_object_font_freetype_deinitialize(&font_freetype);
	}

	

	uint32_t radio_button_row = 5;
	struct 	nhgui_object_radio_button_object radio_button_object[radio_button_row];



	const char *radio_button_text[] = {
		"I accept that you may have my sould.",
		"I agree that the terms of service is good. ",
		"I hope this gui will be useful", 
		"I accept that I lack imagination",
		"I may not have hope and dreams."
	};




	struct nhgui_render_attribute menu_render_attribute = {
		.height_mm = 10,
	};
	struct nhgui_icon_menu_object menu_object;

	/* Search input field */
	const uint32_t input_buffer_size = 32;
	uint32_t input_buffer_length = 0;
	char input_buffer[input_buffer_size];
	struct nhgui_object_input_field input_field;

	/* Add input field */
	const uint32_t add_buffer_size = 32;
	uint32_t add_buffer_length = 0;
	char add_buffer[input_buffer_size];
	struct nhgui_object_input_field add_field;

	struct nhgui_object_text_list_object list_object;
	uint32_t list_entries_length[] = 
	{
		strlen(radio_button_text[0]), 	
		strlen(radio_button_text[1]), 	
		strlen(radio_button_text[2]), 	
		strlen(radio_button_text[3]), 	
		strlen(radio_button_text[4]), 	
	};

	struct nhgui_result render_result = {};
	struct nhgui_glfw_frame frame = nhgui_frame_create();

	while(!glfwWindowShouldClose(window))
	{

		struct nhgui_input input = nhgui_glfw_frame_begin(&frame, window);

		glViewport(0, 0, input.width, input.height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		struct nhgui_result render_result_start = render_result;
		render_result_start.x_mm = 0;
		render_result_start.y_mm = context.height_mm * (float)input.height/(float)context.res_y;
		
		
		/* Menu button */
		render_result = nhgui_result_margin(render_result_start, 1, 1);
		struct nhgui_result m_render_result = nhgui_icon_menu(&context, &menu_object, &menu_render_attribute, &input, render_result);

		render_result = nhgui_result_dec_y(m_render_result);
		m_render_result = nhgui_result_inc_x(m_render_result);

		/* Menu button text */
		const char menu_text[] = "menu";
		render_result = nhgui_result_margin(render_result, 0, 1);
		struct nhgui_result c_render_result = nhgui_object_font_text_result_centered_by_previous_x(
				render_result, 
				&context, 
				radio_character, 
				menu_text,
				sizeof(menu_text)
		);


		render_result = nhgui_object_font_text(
				&context, 
				radio_character, 
				menu_text,
				sizeof(menu_text),
				&radio_render_attribute,
				&input, 
				c_render_result
		);

		render_result = nhgui_result_dec_y(render_result);

		struct nhgui_result res = nhgui_object_input_field(
				&context,
				&input_field, 
				radio_character, 
				&radio_render_attribute,
				&input, 
				m_render_result,
				input_buffer, 
				&input_buffer_length,
				input_buffer_size
		);
		
		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, m_render_result); 
		res = nhgui_result_margin(res, 0, 1);	
			
		res = nhgui_object_input_field(
				&context,
				&add_field, 
				radio_character, 
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
				&context, 
				&list_object,
				radio_button_text,
				list_entries_length,
				5, 
				radio_character, 
				&radio_render_attribute,
				&input, 
				res
		);


		
	



		if(menu_object.clicked)
		{
			struct nhgui_result radio_init_result = c_render_result;
			for(uint32_t j = 0; j < radio_button_row; j++)
			{
				render_result.x_mm = radio_init_result.x_mm;

				render_result = nhgui_result_margin(render_result, 0, 1);

				uint32_t index = j;
				render_result = nhgui_object_radio_button(
						&context,
						&radio_button_object[index], 
						&radio_render_attribute, 
						&input,
					       	render_result
				);
				
				render_result = nhgui_result_inc_x(render_result);
				render_result = nhgui_result_margin(render_result, 2, 0);

				render_result = nhgui_object_font_text(
						&context, 
						radio_character, 
						radio_button_text[j], 
						strlen(radio_button_text[j]), 
						&radio_render_attribute,
						&input, 
						render_result
				);

				
				render_result = nhgui_result_margin(render_result, 0, 1);

				render_result = nhgui_result_rewind_x(render_result);
				render_result = nhgui_result_dec_y(render_result);

			}
					
		}
		
		const char test_string[] = "Test string";

		nhgui_object_font_text(
				&context, 
				character, 
				test_string, 
				sizeof(test_string), 
				&font_render_attribute,
				&input, 
				render_result
		);

		nhgui_glfw_frame_end(&frame, &input);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
