#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>


#include "context.h"
#include "result.h"
#include "shader.h"


#include "misc/file.h"



#include "object/radio_button.h"

int main(int args, char *argv[])
{
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

	glfwMakeContextCurrent(window);
	
	/* Disable vertical sync */
	glfwSwapInterval(0);

  	
       	/* Enable Version 3.3 */
	glewExperimental = 1;
	if (glewInit() != GLEW_OK)
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

	struct nhgui_context context;
	result = nhgui_context_initialize(&context, (uint32_t)res_x, (uint32_t)res_y, (uint32_t)width_mm, (uint32_t)height_mm);
	if(result < 0){
		fprintf(stderr, "nhgui_context_initialize() failed. \n");
		exit(EXIT_FAILURE);
	}

	struct nhgui_render_attribute render_attribute = {
		.height_mm = 3,
	};
	
	struct nhgui_object_radio_button_instance radio_button_instance;
	result = nhgui_object_radio_button_initialize(&radio_button_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_object_radio_button_initialize() failed. \n");
		exit(EXIT_FAILURE);	
	}
	
	uint32_t radio_button_col = 5;
	uint32_t radio_button_row = 5;
	uint32_t radio_button_count = 5*5;	
	struct 	nhgui_object_radio_button_object radio_button_object[radio_button_count];

	int mouse_button_last = GLFW_RELEASE;	
	while(!glfwWindowShouldClose(window))
	{
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
			.cursor_y = (uint32_t)y_cursor,
			.cursor_button_left = mouse_button_state
		};	

		struct nhgui_result render_result = {};
		

		glViewport(0, 0, width,height);
		glClearColor(0.5, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		for(uint32_t i = 0; i < radio_button_col; i++)
		{
			for(uint32_t j = 0; j < radio_button_row; j++)
			{
				uint32_t index = i*radio_button_row + j;
				render_result = nhgui_object_radio_button(&context, &radio_button_instance, &radio_button_object[index], &render_attribute, &input, render_result);
				render_result = nhgui_result_next_y(render_result);
			}
			
			render_result = nhgui_result_rewind_y(render_result);
			render_result = nhgui_result_next_x(render_result);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_object_radio_button_deinitialize(&radio_button_instance);
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
