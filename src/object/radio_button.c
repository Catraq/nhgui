#include "object/radio_button.h"


struct nhgui_result
nhgui_object_radio_button(
		struct nhgui_context *context,
	       	struct nhgui_object_radio_button_instance *instance,
	       	struct nhgui_object_radio_button_object *object,
	       	struct nhgui_render_attribute *attribute,
	       	struct nhgui_input *input,
	       	struct nhgui_result result
)
{	

	/* Calculate coordinates of the cursor in mm.
	 * First crate a scale relative to how large the application window is in comparison to the scrren size 
	 * then calculate pixels per mm  and mutiply by cursor position.
	 * */

	float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
	float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;

	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;
	
	/* It is a radio button. Calcaute distance from center to know if the mouse is over it */	
	float  center_x = result_tmp.x_mm + attribute->height_mm/2;
	float  center_y = result_tmp.y_mm + attribute->height_mm/2;	
	float  distance = (center_x - cursor_x_mm)*(center_x - cursor_x_mm) + (center_y - cursor_y_mm)*(center_y - cursor_y_mm);
	
	/* Keep in mind that the values are squared */
	if(distance < attribute->height_mm*attribute->height_mm/4)
	{
		if(input->cursor_button_left > 0)
		{
			object->checked = object->checked ? 0 : 1;		
		}		
	}
	



	/* Scale by window relative resolution and calcuate mm per 1.0 unit mul with actual height and width */	
	float s_x = (float)context->res_x/(float)input->width * 1.0 /(float)context->width_mm * (float)attribute->height_mm;
	float s_y = (float)context->res_y/(float)input->height * 1.0 /(float)context->height_mm * (float)attribute->height_mm;

	float p_y = (float)context->res_y/(float)input->height * 1.0/(float)context->height_mm * result_tmp.y_mm;
	float p_x = (float)context->res_x/(float)input->width * 1.0/(float)context->width_mm * result_tmp.x_mm; ;
	
	/* Convert to gl cordinates [-1, 1] and move down with size otherwise the element will be above the screen */ 
	p_y = 2.0*p_y-1.0;
	p_x = 2.0*p_x-1.0;
	
	struct nhgui_result render_result = result;
	render_result.y_inc_next = attribute->height_mm;
	render_result.x_inc_next = attribute->height_mm;

	glUseProgram(instance->shader_program);	

	glUniform1ui(instance->location_checked, object->checked);
	glUniform2f(instance->location_position, p_x, p_y);
	glUniform2f(instance->location_size, s_x, s_y);
	glUniform2ui(instance->location_dimension, input->width, input->height);

	nhgui_surface_render(&context->surface);

	return render_result;
}

int nhgui_object_radio_button_initialize(struct nhgui_object_radio_button_instance *instance)
{
	const char *vertex_source_filename = "../shaders/radio_button.vs";
	const char *fragment_source_filename = "../shaders/radio_button.fs";

	uint8_t vertex_source[NGGUI_SHADER_FILE_MAX_SIZE];
	uint8_t fragment_source[NGGUI_SHADER_FILE_MAX_SIZE];

	uint32_t vertex_read = misc_file_read_buffer(vertex_source_filename, vertex_source, NGGUI_SHADER_FILE_MAX_SIZE);
	if(vertex_read == 0){
		fprintf(stderr, "Could not read file %s \n", vertex_source_filename);
		return -1;
	}

	uint32_t fragment_read = misc_file_read_buffer(fragment_source_filename, fragment_source, NGGUI_SHADER_FILE_MAX_SIZE);
	if(fragment_read == 0){
		fprintf(stderr, "Could not read file %s \n", fragment_source_filename);
		return -1;
	}

	const char *vertex_source_list[] = {
		(const char*)vertex_source	
	};	

	const char *fragment_source_list[] = {
		(const char*)fragment_source	
	};	

	GLuint program = nhgui_shader_vertex_create(
			vertex_source_list, NULL, 1,
			fragment_source_list, NULL, 1
	);

	if(program == 0)
	{
		fprintf(stderr, "Could not create shader program. \n");
		return -1;	
	
	}

	const char *checked_uniform_str = "checked";
	GLint checked_location = glGetUniformLocation(program, checked_uniform_str);
	if(checked_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", checked_uniform_str);
		
		glDeleteProgram(program);

		return -1;	
	}

	const char *position_uniform_str = "position";
	GLint position_location = glGetUniformLocation(program, position_uniform_str);
	if(position_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", position_uniform_str);
		
		glDeleteProgram(program);

		return -1;	
	}

	const char *size_uniform_str = "size";
	GLint size_location = glGetUniformLocation(program, size_uniform_str);
	if(size_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", size_uniform_str);
		
		glDeleteProgram(program);

		return -1;	
	}

	const char *dimension_uniform_str = "dimension";
	GLint dimension_location = glGetUniformLocation(program, dimension_uniform_str);
	if(dimension_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", dimension_uniform_str);
		
		glDeleteProgram(program);

		return -1;	
	}


	instance->shader_program = program;
	instance->location_dimension = dimension_location;
	instance->location_checked = checked_location;
	instance->location_position = position_location;
	instance->location_size = size_location;

	return 0;
}

void nhgui_object_radio_button_deinitialize(struct nhgui_object_radio_button_instance *instance)
{
	glDeleteProgram(instance->shader_program);
}




