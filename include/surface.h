#ifndef NHGUI_SURFACE_H
#define NHGUI_SURFACE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct nhgui_surface
{
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
	GLuint draw_count;
};

int nhgui_surface_initialize(
		struct nhgui_surface *surface
);

void nhgui_surface_deinitialize(
		struct nhgui_surface *surface
);

void nhgui_surface_render(
		struct nhgui_surface *nhgui_surface
);

void nhgui_surface_render_instanced(
		struct nhgui_surface *nhgui_surface,
	       	uint32_t instance_count
);


#endif 

