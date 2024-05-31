#include "icon/menu.h"


struct nhgui_result
nhgui_icon_menu(
		struct nhgui_context *context, 
		struct nhgui_icon_menu_instance *instance,
		struct nhgui_icon_menu_object *object,
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result
)
{
	float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
	float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;
	
	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;

	if(input->cursor_button_left > 0)
	{
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->height_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			object->clicked = object->clicked ? 0 : 1;	
		}	
	
	}
	
	glUseProgram(instance->program);	

	nhgui_common_uniform_locations_set(&instance->locations, context, input, result_tmp, attribute->height_mm, attribute->height_mm);

	nhgui_surface_render(&context->surface);

	struct nhgui_result render_result = result;
	render_result.y_inc_next = attribute->height_mm;
	render_result.x_inc_next = attribute->height_mm;


	return render_result;
}


int 
nhgui_icon_menu_initialize(struct nhgui_icon_menu_instance *instance)
{
	const char *vertex_source_filename = "../shaders/menu.vs";
	const char *fragment_source_filename = "../shaders/menu.fs";

	instance->program = nhgui_shader_vertex_create_from_file(
			vertex_source_filename,
			fragment_source_filename
	);

	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_file() failed. \n");
		return -1;	
	}
	
	int result = nhgui_common_uniform_locations_find(&instance->locations, instance->program);
	if(result < 0)
	{
		fprintf(stderr, "nhgui_common_uniform_locations_find() failed. \n");
		glDeleteProgram(instance->program);
		return -1;
	}


	return 0;
}

void
nhgui_icon_menu_deinitialize(struct nhgui_icon_menu_instance *instance)
{
	glDeleteProgram(instance->program);
}
