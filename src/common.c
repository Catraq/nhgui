#include "common.h"


int
nhgui_common_uniform_locations_find(struct nhgui_common_uniform_locations *locations, GLuint program)
{
	const char *position_uniform_str = "position";
	GLint position_location = glGetUniformLocation(program, position_uniform_str);
	if(position_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", position_uniform_str);
		
		return -1;	
	}

	const char *size_uniform_str = "size";
	GLint size_location = glGetUniformLocation(program, size_uniform_str);
	if(size_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", size_uniform_str);
		
		return -1;	
	}

	const char *dimension_uniform_str = "dimension";
	GLint dimension_location = glGetUniformLocation(program, dimension_uniform_str);
	if(dimension_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", dimension_uniform_str);
		
		return -1;	
	}

	locations->position = position_location;
	locations->size = size_location;
	locations->dimension = dimension_location;

	return 0;

}

void
nhgui_common_uniform_locations_set(struct nhgui_common_uniform_locations *locations, struct nhgui_context *context, struct nhgui_input *input, struct nhgui_result result, uint32_t width_mm, uint32_t height_mm)
{
	/* Scale by window relative resolution and calcuate mm per 1.0 unit mul with actual height and width */	
	float s_x = (float)context->res_x/(float)input->width * 1.0 /(float)context->width_mm * width_mm;
	float s_y = (float)context->res_y/(float)input->height * 1.0 /(float)context->height_mm * height_mm;

	/* Negative as we grow down. */	
	float p_y = (float)context->res_y/(float)input->height * 1.0/(float)context->height_mm * result.y_mm;
	float p_x = (float)context->res_x/(float)input->width * 1.0/(float)context->width_mm * result.x_mm; ;
	
	/* Convert to gl cordinates [-1, 1] and move down with size otherwise the element will be above the screen */ 
	p_y = 2.0*p_y-1.0;
	p_x = 2.0*p_x-1.0;
	
	glUniform2f(locations->position, p_x, p_y);
	glUniform2f(locations->size, s_x, s_y);
	glUniform2ui(locations->dimension, input->width, input->height);

}


