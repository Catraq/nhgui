#ifndef NHGUI_INPUT_H
#define NHGUI_INPUT_H 

#define NHGUI_INPUT_STDIN_SIZE_MAX 32

struct nhgui_input
{
	uint32_t width;
	uint32_t height;
	uint32_t cursor_x;
	uint32_t cursor_y;
	uint32_t cursor_button_left;
	uint32_t key_backspace_state;

};


#endif 
