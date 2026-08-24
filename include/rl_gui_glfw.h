#ifndef RL_GUI_GLFW_H
#define RL_GUI_GLFW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include <sys/time.h>

#include  "rl_gui.h"

#define GLFW_CHARACTER_CALLBACK_BUFFER_SIZE 32 

struct rl_gui_glfw_frame 
{
	float prev_cursor_x;
	float prev_cursor_y;

	uint32_t backspace_key_last;
	uint32_t mouse_button_last;
	uint32_t input_selected_new;

	/* > 0 if true  */
	uint32_t key_arrow_left_clicked_last;
	uint32_t key_arrow_right_clicked_last;
	
	struct timeval time_curr_time;
	float total_time;
};


/* Used for creating frame in before it is used in the main loop */
struct rl_gui_glfw_frame
rl_gui_frame_create(GLFWwindow *window);

/* Placed in the end of the rendering loop. */
void 
rl_gui_glfw_frame_end(struct rl_gui_glfw_frame *frame, struct rl_gui_input *input);

/* Placed in the beginning of the rendering loop. */
struct rl_gui_input 
rl_gui_glfw_frame_begin(struct rl_gui_glfw_frame *frame, GLFWwindow *window);

/* Callback that should be used by glfw */
void 
rl_gui_glfw_char_callback(GLFWwindow *window, unsigned int codepoint);

#endif 
