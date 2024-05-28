#include "context.h"


int nhgui_context_initialize(
		struct nhgui_context *context,
	       	uint32_t res_x, uint32_t res_y,
	       	uint32_t width_mm, uint32_t height_mm
)
{
	int result = 0;

	context->width_mm = width_mm;
	context->height_mm = height_mm;
	context->res_x = res_x;
	context->res_y = res_y;

	result = nhgui_surface_initialize(&context->surface);
	if(result < 0){
		fprintf(stderr, "nhgui_surface_initialize() failed. \n");
		return -1;
	}

	return 0;
}

void 
nhgui_context_deinitialize(
		struct nhgui_context *context
)
{
	nhgui_surface_deinitialize(&context->surface);
}

