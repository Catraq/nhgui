#ifndef NHGUI_ICON_MENU_H
#define NHGUI_ICON_MENU_H

#include "context.h"
#include "result.h"
#include "shader.h"
#include "common.h"
#include "attribute.h"


struct nhgui_icon_menu_instance
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};

struct nhgui_icon_menu_object
{
	uint8_t clicked;
};


struct nhgui_result
nhgui_icon_menu(
		struct nhgui_context *context, 
		struct nhgui_icon_menu_instance *instance,
		struct nhgui_icon_menu_object *object,
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result
);


int 
nhgui_icon_menu_initialize(
		struct nhgui_icon_menu_instance *instance
);

void
nhgui_icon_menu_deinitialize(
		struct nhgui_icon_menu_instance *instance
);


#endif 
