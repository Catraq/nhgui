#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>


#include "context.h"
#include "result.h"
#include "shader.h"
#include "common.h"


#include "misc/file.h"

#include "icon/menu.h"
#include "object/radio_button.h"
#include "object/font.h"

#define GLFW_CHARACTER_CALLBACK_BUFFER_SIZE 32


uint32_t character_callback_buffer_index;
char character_callback_buffer[GLFW_CHARACTER_CALLBACK_BUFFER_SIZE];

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
	//ANSI CHARACTERS 
	if(codepoint > 30 && codepoint < 127){
		if(character_callback_buffer_index < GLFW_CHARACTER_CALLBACK_BUFFER_SIZE)
		{
			character_callback_buffer[character_callback_buffer_index] = codepoint;
			character_callback_buffer_index++;
		}
	}
}

uint32_t
nhgui_input_text(char *buffer, uint32_t buffer_length, uint32_t buffer_size)
{
	for(uint32_t i = 0; i < character_callback_buffer_index; i++)
	{
		if(buffer_length + i < buffer_size)
			buffer[buffer_length + i] = character_callback_buffer[i];
		else
		{
			character_callback_buffer_index -= i;
			return buffer_length + i;
		}

	}
	
	uint32_t ret =	buffer_length + character_callback_buffer_index;
	character_callback_buffer_index = 0;
	return ret;
}


struct nhgui_result 
nhgui_object_input_field(
		struct nhgui_context *context,
		struct nhgui_font_text_instance *instance,
		struct nhgui_font_character character[128], 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result,
		char *input_buffer, 
		uint32_t *input_buffer_length,
		uint32_t input_buffer_size
)
{

	/* Search input */
	*input_buffer_length = nhgui_input_text(input_buffer, *input_buffer_length, input_buffer_size);
	if(input->key_backspace_state  > 0){
		*input_buffer_length -= 1;	
	} 

	return nhgui_font_text(
			context, 
			instance,
			character, 
			input_buffer,
			*input_buffer_length,
			attribute,
			input, 
			result
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


	character_callback_buffer_index = 0;
	glfwSetCharCallback(window, character_callback);

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

	
	/* Initialize instances of the different "tools" */
	
	struct nhgui_object_radio_button_instance radio_button_instance;
	result = nhgui_object_radio_button_initialize(&radio_button_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_object_radio_button_initialize() failed. \n");
		exit(EXIT_FAILURE);	
	}



	struct nhgui_icon_menu_instance menu_instance;
	result = nhgui_icon_menu_initialize(&menu_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_icon_menu_initialize() failed. \n");
		exit(EXIT_FAILURE);	
	}


	
	struct nhgui_render_attribute font_render_attribute = {
		.height_mm = 10,
	};

	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
	};



	const char *font_filename = "../data/UbuntuMono-R.ttf";
	struct nhgui_font_character character[128];
	struct nhgui_font_character radio_character[128];

	{
		struct nhgui_font_freetype font_freetype;
		result = nhgui_font_freetype_initialize(&font_freetype);
		if(result < 0){
			fprintf(stderr, "nhgui_font_freetype_initilaize() failed. \n");
			exit(EXIT_FAILURE);
		}
		
		result = nhgui_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&font_render_attribute ,
				character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}
	
		result = nhgui_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&radio_render_attribute,
				radio_character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}


		nhgui_font_freetype_deinitialize(&font_freetype);
	}

	struct nhgui_font_text_instance font_text_instance;
	result = nhgui_font_text_initialize(&font_text_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_font_text_initialize() failed. \n");
		exit(EXIT_FAILURE);
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
	memset(input_buffer, 'A', input_buffer_size);




	int backspace_key_last = GLFW_RELEASE;
	int mouse_button_last = GLFW_RELEASE;	
	while(!glfwWindowShouldClose(window))
	{
		int backspace_key = glfwGetKey(window, GLFW_KEY_BACKSPACE);
		uint32_t backspace_key_state  = backspace_key == GLFW_RELEASE ?  backspace_key_last != backspace_key ? 1 : 0 : 0;
		backspace_key_last = backspace_key;


		/* Get screen pixel size */
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		/* Get input from mouse and mouse buttons */
		double x_cursor, y_cursor;
		glfwGetCursorPos(window, &x_cursor, &y_cursor);

		int mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		uint32_t mouse_button_state = mouse_button == GLFW_RELEASE ?  mouse_button_last != mouse_button ? 1 : 0 : 0;
		mouse_button_last = mouse_button;
		
		struct nhgui_input input = {
			.width = width,
			.height = height,
			.cursor_x = (uint32_t)x_cursor,
			.cursor_y = height - (uint32_t)y_cursor,
			.cursor_button_left = mouse_button_state,
			.key_backspace_state = backspace_key_state
		};	


		struct nhgui_result render_result = {
			.x_mm = 0,
			.y_mm = context.height_mm * (float)input.height/(float)context.res_y
		};
		

		glViewport(0, 0, width,height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		/* Menu button */
		render_result = nhgui_result_margin(render_result, 1, 1);
		struct nhgui_result m_render_result = nhgui_icon_menu(&context, &menu_instance, &menu_object, &menu_render_attribute, &input, render_result);

		render_result = nhgui_result_dec_y(m_render_result);
		m_render_result = nhgui_result_inc_x(m_render_result);

		/* Menu button text */
		const char menu_text[] = "menu";
		render_result = nhgui_result_margin(render_result, 0, 1);
		struct nhgui_result c_render_result = nhgui_font_text_result_centered_by_previous_x(
				render_result, 
				&context, 
				radio_character, 
				menu_text,
				sizeof(menu_text)
		);


		render_result = nhgui_font_text(
				&context, 
				&font_text_instance,
				radio_character, 
				menu_text,
				sizeof(menu_text),
				&radio_render_attribute,
				&input, 
				c_render_result
		);

		render_result = nhgui_result_dec_y(render_result);

		nhgui_object_input_field(
				&context,
				&font_text_instance,
				radio_character, 
				&radio_render_attribute,
				&input, 
				m_render_result,
				input_buffer, 
				&input_buffer_length,
				input_buffer_size
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
					       	&radio_button_instance, 
						&radio_button_object[index], 
						&radio_render_attribute, 
						&input,
					       	render_result
				);
				
				render_result = nhgui_result_inc_x(render_result);
				render_result = nhgui_result_margin(render_result, 2, 0);

				render_result = nhgui_font_text(
						&context, 
						&font_text_instance,
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

		nhgui_font_text(
				&context, 
				&font_text_instance,
				character, 
				test_string, 
				sizeof(test_string), 
				&font_render_attribute,
				&input, 
				render_result
		);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_object_radio_button_deinitialize(&radio_button_instance);
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
