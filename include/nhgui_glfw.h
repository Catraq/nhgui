#ifndef NHGUI_GLFW_H
#define NHGUI_GLFW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>

#include  "nhgui.h"

#define GLFW_CHARACTER_CALLBACK_BUFFER_SIZE 32 

struct nhgui_glfw_frame 
{
	float prev_cursor_x;
	float prev_cursor_y;

	uint32_t backspace_key_last;
	uint32_t mouse_button_last;
	uint32_t input_selected_new;

	struct timeval time_curr_time;
	float total_time;
};


/* Used for creating frame in before it is used in the main loop */
struct nhgui_glfw_frame
nhgui_frame_create(GLFWwindow *window);

/* Placed in the end of the rendering loop. */
void 
nhgui_glfw_frame_end(struct nhgui_glfw_frame *frame, struct nhgui_input *input);

/* Placed in the beginning of the rendering loop. */
struct nhgui_input 
nhgui_glfw_frame_begin(struct nhgui_glfw_frame *frame, GLFWwindow *window);

/* Callback that should be used by glfw */
void 
nhgui_glfw_char_callback(GLFWwindow *window, unsigned int codepoint);

#endif 
