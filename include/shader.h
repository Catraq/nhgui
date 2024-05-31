#ifndef NHGUI_SHADER_H
#define NHGUI_SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "misc/file.h"

GLuint nhgui_shader_vertex_create_from_file(
		const char *vertex_source_filename, 
	       	const char *fragment_source_filename
);

GLuint nhgui_shader_vertex_create(
		const char **vertex_source, 
		int32_t *vertex_source_length, 
		uint32_t vertex_source_count, 
		const char **fragment_source,
	       	int32_t *fragment_source_length,
	       	uint32_t fragment_source_count
);

#endif 
