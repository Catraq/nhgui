#ifndef NHGUI_RESULT_H
#define NHGUI_RESULT_H

#include <stdint.h>

struct nhgui_result
{
	/* Current y and x values to draw to */
	float y_mm;
	float x_mm;
	
	/* Next y and x values depending on operation */
	float  y_inc_next;
	float  x_inc_next;
};

struct nhgui_result 
nhgui_result_margin(struct nhgui_result result, float margin_x_mm, float margin_y_mm);

struct nhgui_result
nhgui_result_inc_x(struct nhgui_result result);


struct nhgui_result
nhgui_result_dec_y(struct nhgui_result result);

struct nhgui_result
nhgui_result_rewind_x(struct nhgui_result result);


struct nhgui_result
nhgui_result_rewind_y(struct nhgui_result result);



#endif 
