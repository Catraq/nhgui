#include "nhgui_glfw.h"




uint32_t character_callback_buffer_index = 0;
char character_callback_buffer[NHGUI_INPUT_MAX];

void nhgui_glfw_char_callback(GLFWwindow *window, unsigned int codepoint)
{
	if(codepoint > 30 && codepoint < 127){
		if(character_callback_buffer_index < NHGUI_INPUT_MAX)
		{
			character_callback_buffer[character_callback_buffer_index] = codepoint;
			character_callback_buffer_index++;
		}
	}
}

void character_callback_erase_char()
{
	if(character_callback_buffer_index > 0)
		character_callback_buffer_index--;
}
void
character_callback_erase_buffer()
{
	character_callback_buffer_index = 0;
}

struct nhgui_glfw_frame
nhgui_frame_create(GLFWwindow *window)
{
	struct nhgui_glfw_frame frame = {};

	frame.backspace_key_last = GLFW_RELEASE;
	frame.mouse_button_last = GLFW_RELEASE;	
	
	frame.total_time = 0.0f;

	gettimeofday(&frame.time_curr_time, NULL);
	
	frame.input_selected_new = 0;	
	
	double x_cursor, y_cursor;
	glfwGetCursorPos(window, &x_cursor, &y_cursor);
	frame.prev_cursor_x = x_cursor;
	frame.prev_cursor_y = y_cursor;

	return frame;	
}

void 
nhgui_glfw_frame_end(struct nhgui_glfw_frame *frame, struct nhgui_input *input)
{
	frame->input_selected_new = input->selected_new_raise;
}

struct nhgui_input 
nhgui_glfw_frame_begin(struct nhgui_glfw_frame *frame, GLFWwindow *window)
{

	struct timeval time_curr_tmp;
	gettimeofday(&time_curr_tmp, NULL);
	float deltatime = time_curr_tmp.tv_sec - frame->time_curr_time.tv_sec + (float)(time_curr_tmp.tv_usec - frame->time_curr_time.tv_usec)/(1000.0f*1000.0f);
	frame->time_curr_time = time_curr_tmp;
	frame->total_time += deltatime;



	int backspace_key = glfwGetKey(window, GLFW_KEY_BACKSPACE);
	uint32_t backspace_key_state  = backspace_key == GLFW_RELEASE ?  frame->backspace_key_last != backspace_key ? 1 : 0 : 0;
	frame->backspace_key_last = backspace_key;
	if(frame->backspace_key_last)
	{
		character_callback_erase_char();
	}

	/* Get screen pixel size */
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	/* Get input from mouse and mouse buttons */
	double x_cursor, y_cursor;
	glfwGetCursorPos(window, &x_cursor, &y_cursor);

	int mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	uint32_t mouse_button_state = mouse_button == GLFW_RELEASE ?  frame->mouse_button_last != mouse_button ? 1 : 0 : 0;

	uint32_t mouse_button_left_pressed = (frame->mouse_button_last == mouse_button && mouse_button == GLFW_PRESS);

	frame->mouse_button_last = mouse_button;
	
	struct nhgui_input input = {
		.width = width,
		.height = height,
		.cursor_x = (uint32_t)x_cursor,
		.cursor_y = height - (uint32_t)y_cursor,
		.cursor_x_delta = x_cursor - frame->prev_cursor_x,
		.cursor_y_delta = height - y_cursor - frame->prev_cursor_y,
		.cursor_button_left = mouse_button_state,
		.cursor_button_left_press = mouse_button_left_pressed, 
		.key_backspace_state = backspace_key_state,
		.deltatime = deltatime,
		.time = frame->total_time,
		.selected_new = frame->input_selected_new,
		.selected_new_raise = 0,

	};

	frame->input_selected_new = 0;
	frame->prev_cursor_y = height - 
y_cursor;
	frame->prev_cursor_x = x_cursor;

	memcpy(input.input, character_callback_buffer, character_callback_buffer_index); 
	input.input_length = character_callback_buffer_index;
	character_callback_erase_buffer();	


	return input;		
}


