#ifndef NHGUI_SHADER_H
#define NHGUI_SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint nhgui_shader_vertex_create(
		const char **vertex_source, 
		uint32_t *vertex_source_length, 
		uint32_t vertex_source_count, 
		const char **fragment_source,
	       	uint32_t *fragment_source_length,
	       	uint32_t fragment_source_count
);

#endif 
