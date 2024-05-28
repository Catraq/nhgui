#include "result.h"

struct nhgui_result
nhgui_result_next_x(struct nhgui_result result)
{
	result.x_mm = result.x_mm + result.x_inc_next;
	result.x_inc_next = 0;

	return result;

}

struct nhgui_result
nhgui_result_next_y(struct nhgui_result result)
{
	result.y_mm = result.y_mm + result.y_inc_next;
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
