#ifndef NHGUI_CONTEXT_H
#define  NHGUI_CONTEXT_H

#include "surface.h"

struct nhgui_context
{
	struct nhgui_surface surface;

	/* Screen width and height in mm. */
	uint32_t width_mm;
	uint32_t height_mm;
	
	/* Screen resolution */
	uint32_t res_x;
	uint32_t res_y;
};

int nhgui_context_initialize(
		struct nhgui_context *context,
	       	uint32_t res_x, uint32_t res_y,
	       	uint32_t width_mm, uint32_t height_mm
);

void nhgui_context_deinitialize(
		struct nhgui_context *context
);

#endif 
