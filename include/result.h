#ifndef NHGUI_RESULT_H
#define NHGUI_RESULT_H

#include <stdint.h>

struct nhgui_result
{
	/* Current y and x values to draw to */
	uint32_t y_mm;
	uint32_t x_mm;
	
	/* Next y and x values depending on operation */
	uint32_t y_inc_next;
	uint32_t x_inc_next;
};

struct nhgui_result
nhgui_result_next_x(struct nhgui_result result);


struct nhgui_result
nhgui_result_next_y(struct nhgui_result result);

struct nhgui_result
nhgui_result_rewind_x(struct nhgui_result result);


struct nhgui_result
nhgui_result_rewind_y(struct nhgui_result result);



#endif 
