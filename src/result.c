#include "result.h"

struct nhgui_result 
nhgui_result_margin(struct nhgui_result result, float margin_x_mm, float margin_y_mm)
{
	result.x_mm += margin_x_mm;
	result.y_mm -= margin_y_mm;
	return result;
}


struct nhgui_result
nhgui_result_inc_x(struct nhgui_result result)
{
	result.x_mm = result.x_mm + result.x_inc_next;
	result.x_inc_next = 0;

	return result;

}

struct nhgui_result
nhgui_result_dec_y(struct nhgui_result result)
{
	result.y_mm = result.y_mm - result.y_inc_next;
	result.y_inc_next = 0;

	return result;

}

struct nhgui_result
nhgui_result_rewind_x(struct nhgui_result result)
{
	result.x_mm = 0;
	return result;

}

struct nhgui_result
nhgui_result_rewind_y(struct nhgui_result result)
{
	result.y_mm = 0;
	return result;

}
