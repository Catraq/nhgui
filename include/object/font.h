#ifndef NHGUI_OBJECT_FONT_H
#define NHGUI_OBJECT_FONT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "context.h"
#include "result.h"
#include "shader.h"
#include "common.h"
#include "attribute.h"



struct nhgui_font_freetype
{
	FT_Library ft;
};

struct nhgui_font_character
{
	/* Opengl texture object */
	GLuint texture;

	/* Width and height of character in pixels */
	uint32_t width;
	uint32_t height;

	/* See freetype glyph documentation */
	uint32_t bearing_x;
	uint32_t bearing_y;
	uint32_t advance_x;	
};

struct nhgui_font_text_instance 
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};


int 
nhgui_font_freetype_initialize(struct nhgui_font_freetype *freetype);

void 
nhgui_font_freetype_deinitialize(struct nhgui_font_freetype *freetype);

int 
nhgui_font_freetype_characters_initialize(
		struct nhgui_font_freetype *freetype,
		struct nhgui_context *context,
		struct nhgui_render_attribute *attribute,
	       	struct nhgui_font_character character[128], 
	       	const char *filename
);

void 
nhgui_font_freetype_characters_deinitialize(
	       	struct nhgui_font_character character[128]
);

int 
nhgui_font_text_initialize(struct nhgui_font_text_instance *instance);

void
nhgui_font_text_deinitialize(struct nhgui_font_text_instance *instance);

struct nhgui_result
nhgui_font_text_result_centered_by_previous_x(
		struct nhgui_result result,
		struct nhgui_context *context, 
		struct nhgui_font_character character[128],
		const char *text,
		uint32_t text_length
);



struct nhgui_result
nhgui_font_text(
		struct nhgui_context *context, 
		struct nhgui_font_text_instance *instance,
		struct nhgui_font_character character[128], 
		const char *text, 
		uint32_t text_length, 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result
);



#endif 
