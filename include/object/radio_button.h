#ifndef NHGUI_OBJECT_RADIO_BUTTON_H
#define NHGUI_OBJECT_RADIO_BUTTON_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>

#include "shader.h"
#include "config.h"
#include "context.h"
#include "attribute.h"
#include "input.h"
#include "result.h"



struct nhgui_object_radio_button_instance 
{
	GLuint shader_program;

	GLint location_dimension;
	GLint location_checked;
	GLint location_position;
	GLint location_size;
};

struct nhgui_object_radio_button_object
{
	uint8_t checked;
};



struct nhgui_result
nhgui_object_radio_button(
		struct nhgui_context *context,
	       	struct nhgui_object_radio_button_instance *instance,
	       	struct nhgui_object_radio_button_object *object,
	       	struct nhgui_render_attribute *attribute,
	       	struct nhgui_input *input,
	       	struct nhgui_result result
);

int nhgui_object_radio_button_initialize(
		struct nhgui_object_radio_button_instance *instance
);


void nhgui_object_radio_button_deinitialize(
		struct nhgui_object_radio_button_instance *instance
);

#endif

