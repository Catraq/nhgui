#ifndef NHGUI_COMMON_H
#define NHGUI_COMMON_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "context.h"
#include "input.h"
#include "result.h"

struct nhgui_common_uniform_locations
{
	GLint position;
	GLint size;
	GLint dimension;
};


int nhgui_common_uniform_locations_find(
		struct nhgui_common_uniform_locations *locations, 
		GLuint program
);

void nhgui_common_uniform_locations_set(
		struct nhgui_common_uniform_locations *locations,
	       	struct nhgui_context *context,
	       	struct nhgui_input *input,
	       	struct nhgui_result result,
	       	uint32_t width_mm, uint32_t height_mm
);
#endif 
