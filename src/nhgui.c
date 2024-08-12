#include "nhgui.h"

#include "../config.h"

#define TEXT(x) #x



int nhgui_context_initialize(
		struct nhgui_context *context,
	       	uint32_t screen_resolution_x, uint32_t screen_resolution_y,
	       	uint32_t width_mm, uint32_t height_mm
)
{
	int result = 0;

	context->screen_width_mm = width_mm;
	context->screen_height_mm = height_mm;
	context->screen_resolution_x = screen_resolution_x;
	context->screen_resolution_y = screen_resolution_y;

	/* Common surface used for rendering operations */
	result = nhgui_surface_initialize(&context->surface);
	if(result < 0){
		fprintf(stderr, "nhgui_surface_initialize() failed. \n");
		return -1;
	}
	
	/* Required for the freetype font bitmap from glyph to work */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Initialize the blinking thing used with text input */
	result = nhgui_icon_text_cursor_initialize(&context->text_cursor);
	if(result < 0)
	{
		fprintf(stderr, "nhgui_icon_text_cursor_initialize() failed. \n");

		nhgui_surface_deinitialize(&context->surface);

		return -1;
	}

	/* Initialize the blinking thing used with text input */
	result = nhgui_icon_blank_initialize(&context->blank);
	if(result < 0)
	{
		fprintf(stderr, "nhgui_icon_blank_initialize() failed. \n");
		
		nhgui_icon_text_cursor_deinitialize(&context->text_cursor);
		nhgui_surface_deinitialize(&context->surface);

		return -1;
	}

	result = nhgui_object_font_text_initialize(&context->font);
	if(result < 0){

		nhgui_icon_text_cursor_deinitialize(&context->text_cursor);
		nhgui_surface_deinitialize(&context->surface);
		nhgui_icon_blank_deinitialize(&context->blank);

		fprintf(stderr, "nhgui_object_font_text_initialize() failed. \n");
		return -1;	
	}
	
	result = nhgui_object_radio_button_initialize(&context->radio_button);
	if(result < 0){

		nhgui_icon_text_cursor_deinitialize(&context->text_cursor);
		nhgui_surface_deinitialize(&context->surface);
		nhgui_icon_blank_deinitialize(&context->blank);
		nhgui_object_font_text_deinitialize(&context->font);

		fprintf(stderr, "nhgui_object_radio_button_initialize() failed. \n");
		return -1;
	}



	result = nhgui_icon_menu_initialize(&context->menu);
	if(result < 0){

		nhgui_icon_text_cursor_deinitialize(&context->text_cursor);
		nhgui_surface_deinitialize(&context->surface);
		nhgui_icon_blank_deinitialize(&context->blank);
		nhgui_object_font_text_deinitialize(&context->font);
		nhgui_object_radio_button_deinitialize(&context->radio_button);

		fprintf(stderr, "nhgui_icon_menu_initialize() failed. \n");
		return -1;
	}


	return 0;
}

void 
nhgui_context_deinitialize(
		struct nhgui_context *context
)
{
	nhgui_icon_text_cursor_deinitialize(&context->text_cursor);
	nhgui_surface_deinitialize(&context->surface);
	nhgui_icon_blank_deinitialize(&context->blank);
	nhgui_object_font_text_deinitialize(&context->font);
	nhgui_object_radio_button_deinitialize(&context->radio_button);
	nhgui_icon_menu_deinitialize(&context->menu);
}

int
nhgui_common_uniform_locations_find(struct nhgui_common_uniform_locations *locations, const GLuint program)
{
	const char *position_uniform_str = "position";
	GLint position_location = glGetUniformLocation(program, position_uniform_str);
	if(position_location == -1)
	{
#if NHGUI_DEBUG
		fprintf(stderr, "Could not find uniform location %s. \n", position_uniform_str);
#endif 
	}

	const char *size_uniform_str = "size";
	GLint size_location = glGetUniformLocation(program, size_uniform_str);
	if(size_location == -1)
	{
#if NHGUI_DEBUG
		fprintf(stderr, "Could not find uniform location %s. \n", size_uniform_str);
#endif 
	}

	const char *dimension_uniform_str = "dimension";
	GLint dimension_location = glGetUniformLocation(program, dimension_uniform_str);
	if(dimension_location == -1)
	{
#if NHGUI_DEBUG
		fprintf(stderr, "Could not find uniform location %s. \n", dimension_uniform_str);
#endif 

	}


	const char *color_uniform_str = "color";
	GLint color_location = glGetUniformLocation(program, color_uniform_str);
	if(color_location == -1)
	{
#if NHUI_DEBUG
		fprintf(stderr, "Could not find uniform location %s. \n", color_uniform_str);
#endif 
	}


	locations->position = position_location;
	locations->size = size_location;
	locations->dimension = dimension_location;
	locations->color = color_location;

	return 0;

}

void
nhgui_common_uniform_locations_set(
		const struct nhgui_common_uniform_locations *locations,
	       	const struct nhgui_context *context,
	       	const struct nhgui_input *input,
	       	const struct nhgui_result result,
	       	const float width_mm, 
		const float height_mm, 
		const float r,
	       	const float g,
	       	const float b)
{
	/* Scale by window relative resolution and calcuate mm per 1.0 unit mul with actual height and width */	
	float s_x = (float)context->screen_resolution_x/(float)input->width_pixel * 1.0 /(float)context->screen_width_mm * width_mm;
	float s_y = (float)context->screen_resolution_y/(float)input->height_pixel * 1.0 /(float)context->screen_height_mm * height_mm;

	/* Negative as we grow down. */	
	float p_y = (float)context->screen_resolution_y/(float)input->height_pixel * 1.0/(float)context->screen_height_mm * (result.y_mm + result.y_offset_mm);
	float p_x = (float)context->screen_resolution_x/(float)input->width_pixel * 1.0/(float)context->screen_width_mm * (result.x_mm + result.x_offset_mm);
	
	/* Convert to gl cordinates [-1, 1] and move down with size otherwise the element will be above the screen */ 
	p_y = 2.0*p_y-1.0;
	p_x = 2.0*p_x-1.0;
	
	if(locations->size != -1){	
		glUniform2f(locations->position, p_x, p_y);
		CHECK_GL_ERROR();
	}
	
	if(locations->size != -1){
		glUniform2f(locations->size, s_x, s_y);
		CHECK_GL_ERROR();
	}
	
	if(locations->color != -1){
		glUniform3f(locations->color, r, g, b);
		CHECK_GL_ERROR();
	}
	
	if(locations->dimension !=  -1){
		glUniform2ui(locations->dimension, input->width_pixel, input->height_pixel);
		CHECK_GL_ERROR();
		printf("%i \n", locations->dimension);
	}

}

struct nhgui_result 
nhgui_result_margin(struct nhgui_result result, float margin_x_mm, float margin_y_mm)
{
	result.x_mm += margin_x_mm;
	result.y_mm -= margin_y_mm;
	return result;
}


struct nhgui_result
nhgui_result_inc_x(struct nhgui_result result)
{
	result.x_mm = result.x_mm + result.x_inc_next;
	result.x_inc_next = 0;

	return result;

}

struct nhgui_result
nhgui_result_dec_y(struct nhgui_result result)
{
	result.y_mm = result.y_mm - result.y_inc_next;
	result.y_inc_next = 0;

	return result;

}

struct nhgui_result
nhgui_result_rewind_x(struct nhgui_result result)
{
	result.x_mm = 0;
	return result;

}

struct nhgui_result
nhgui_result_rewind_x_to(struct nhgui_result result, struct nhgui_result to)
{
	result.x_mm = to.x_mm;
	result.x_inc_next = 0;
	return result;
}

GLuint nhgui_shader_vertex_create_from_file(
		const char *vertex_source_filename,
		const char *fragment_source_filename
)
{
	uint8_t vertex_source[NGGUI_SHADER_FILE_MAX_SIZE];
	uint8_t fragment_source[NGGUI_SHADER_FILE_MAX_SIZE];

	uint32_t vertex_read = misc_file_read_buffer(vertex_source_filename, vertex_source, NGGUI_SHADER_FILE_MAX_SIZE);
	if(vertex_read == 0){
		fprintf(stderr, "Could not read file %s \n", vertex_source_filename);
		return 0;
	}

	uint32_t fragment_read = misc_file_read_buffer(fragment_source_filename, fragment_source, NGGUI_SHADER_FILE_MAX_SIZE);
	if(fragment_read == 0){
		fprintf(stderr, "Could not read file %s \n", fragment_source_filename);
		return 0;
	}

	const char *vertex_source_list[] = {
		(const char *)vertex_source	
	};	

	int32_t vertex_source_length[] = {vertex_read};

	const char *fragment_source_list[] = {
		(const char *)fragment_source	
	};	

	int32_t fragment_source_length[] = {fragment_read};

	GLuint program = nhgui_shader_vertex_create(
			vertex_source_list, vertex_source_length, 1,
			fragment_source_list, fragment_source_length, 1
	);

	if(program == 0)
	{
		fprintf(stderr, "Could not create shader program. \n");
		return 0;	
	
	}
	
	return program;
}

GLuint nhgui_shader_vertex_create_from_memory(
		uint8_t *vertex_source, uint32_t vertex_length, 
		uint8_t *fragment_source, uint32_t fragment_length
)
{
	const char *vertex_source_list[] = {
		(const char *)vertex_source	
	};	

	int32_t vertex_source_length[] = {vertex_length};

	const char *fragment_source_list[] = {
		(const char *)fragment_source	
	};	

	int32_t fragment_source_length[] = {fragment_length};

	GLuint program = nhgui_shader_vertex_create(
			vertex_source_list, vertex_source_length, 1,
			fragment_source_list, fragment_source_length, 1
	);

	if(program == 0)
	{
		fprintf(stderr, "Could not create shader program. \n");
		return 0;	
	
	}
	
	return program;


}


GLuint nhgui_shader_vertex_create(
		const char **vertex_source, 
		int32_t *vertex_source_length, 
		uint32_t vertex_source_count, 
		const char **fragment_source,
	       	int32_t *fragment_source_length,
	       	uint32_t fragment_source_count
)
{
	
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	
	glShaderSource(vertex_shader, vertex_source_count, vertex_source, vertex_source_length);
	glShaderSource(fragment_shader, fragment_source_count, fragment_source, fragment_source_length);

	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	
	GLint compiled = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE){
		GLchar log[8192];
		GLsizei length;
		glGetShaderInfoLog(vertex_shader, 8192, &length, log);
		if( length != 0 )
		{
			printf(" ---- Vertexshader compile log ---- \n %s \n", log);
		}

		/* Cleanup and return */	
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return 0;
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE){
		GLchar log[8192];
		GLsizei length;
		glGetShaderInfoLog(fragment_shader, 8192, &length, log);
		if( length != 0 )
		{
			printf(" ---- Fragmentshader compile log ---- \n %s \n", log);
		}

		/* Cleanup and return */	
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return 0;
	}
	
	
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	glLinkProgram(program);
	
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	GLint linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(linked == GL_FALSE) {
		GLsizei length;
		GLchar log[8192];
		glGetProgramInfoLog(program, 8192, &length, log);
		printf(" ---- Program Link log ---- \n %s \n", log);

		glDeleteProgram(program);
		return 0;
	}

	
	return program;
}


const float nhgui_surface_quad_vertices[] = {
	-1.0f, 1.0f, 
	1.0f, 1.0f,
	1.0, -1.0f,
	-1.0f, -1.0f
};	

const GLuint nhgui_surface_quad_vertices_count = sizeof(nhgui_surface_quad_vertices)/sizeof(nhgui_surface_quad_vertices[0]);

const GLuint nhgui_surface_quad_indices[] = {
	1, 0, 2,
	2, 0, 3
};

const GLuint nhgui_surface_quad_indices_count = sizeof(nhgui_surface_quad_indices)/sizeof(nhgui_surface_quad_indices[0]);

int nhgui_surface_initialize(struct nhgui_surface *surface)
{

	GLuint nhgui_surface_vertex_array, nhgui_surface_vertex_buffer, nhgui_surface_element_buffer;

	glGenVertexArrays(1, &nhgui_surface_vertex_array);
	glBindVertexArray(nhgui_surface_vertex_array);

	glGenBuffers(1, &nhgui_surface_element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nhgui_surface_element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(nhgui_surface_quad_indices), nhgui_surface_quad_indices, GL_STATIC_DRAW); 


	glGenBuffers(1, &nhgui_surface_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, nhgui_surface_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nhgui_surface_quad_vertices), nhgui_surface_quad_vertices, GL_STATIC_DRAW); 

	const GLuint nhgui_surface_vertex_index = 0;
	glEnableVertexAttribArray(nhgui_surface_vertex_index);
	glVertexAttribPointer(nhgui_surface_vertex_index, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	surface->vertex_array = nhgui_surface_vertex_array;
	surface->element_buffer = nhgui_surface_element_buffer;
	surface->vertex_buffer = nhgui_surface_vertex_buffer;
	surface->draw_count = nhgui_surface_quad_indices_count;

	return 0;

}


void nhgui_surface_deinitialize(struct nhgui_surface *surface)
{
	glDeleteVertexArrays(1, &surface->vertex_array);
	glDeleteBuffers(1, &surface->element_buffer);
	glDeleteBuffers(1, &surface->vertex_buffer);
}


void nhgui_surface_render(const struct nhgui_surface *nhgui_surface)
{
	glBindVertexArray(nhgui_surface->vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nhgui_surface->element_buffer);
	glDrawElements(GL_TRIANGLE_STRIP, nhgui_surface->draw_count, GL_UNSIGNED_INT, 0);	
	glBindVertexArray(0);

}

void nhgui_surface_render_instanced(const struct nhgui_surface *nhgui_surface, const uint32_t instance_count)
{
	glBindVertexArray(nhgui_surface->vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nhgui_surface->element_buffer);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, nhgui_surface->draw_count, GL_UNSIGNED_INT, 0, instance_count);	
	glBindVertexArray(0);
}


struct nhgui_result 
nhgui_icon_blank_no_object(
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
)
{
	const struct nhgui_icon_blank_instance *instance = &context->blank;


	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;

	glUseProgram(instance->program);	
	CHECK_GL_ERROR();

	nhgui_common_uniform_locations_set(
			&instance->locations,
		       	context,
		       	input,
		       	result_tmp,
		       	attribute->width_mm, attribute->height_mm,
			attribute->r, attribute->g, attribute->b
	);
	CHECK_GL_ERROR();

	nhgui_surface_render(&context->surface);
	CHECK_GL_ERROR();


	
	struct nhgui_result r = result;
	r.y_inc_next = attribute->height_mm;
	r.x_inc_next = attribute->width_mm;

	r.y_min_mm = r.y_min_mm < r.y_mm - r.y_inc_next ? r.y_min_mm : r.y_mm - r.y_inc_next;
	r.x_max_mm = r.x_max_mm < r.x_mm + r.x_inc_next ? r.x_mm + r.x_inc_next : r.x_max_mm;

	return r;
}

struct nhgui_result 
nhgui_icon_blank(
		struct nhgui_icon_blank *blank,
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		const struct nhgui_result result
)
{
	const struct nhgui_icon_blank_instance *instance = &context->blank;

	float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
	float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;
	
	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;
	
	blank->clicked = 0;
	if(input->cursor_button_left > 0)
	{
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			blank->clicked = 1;
		}	
	
	}

	if(input->cursor_button_left_press > 0)
	{
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			blank->pressed = 1;
		}	
	
	}
	else
	{
		blank->pressed = 0;	
	}

	blank->deselected = 0;


	if(blank->selected_prev > 0)
	{
		blank->selected_prev = 0;
	}
	else if(input->selected_new > 0)
	{
		blank->deselected = 1;
		blank->selected = 0;	
	}	
	else if(input->cursor_button_left > 0)
	{
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			blank->selected = 1;
			blank->selected_prev = 1;
			input->selected_new_raise = 1;	
		}	
	
	}
	

	glUseProgram(instance->program);	

	nhgui_common_uniform_locations_set(
			&instance->locations,
		       	context,
		       	input,
		       	result_tmp,
		       	attribute->width_mm, attribute->height_mm,
			attribute->r, attribute->g, attribute->b
	);

	nhgui_surface_render(&context->surface);
	
	struct nhgui_result r = result;
	r.y_inc_next = attribute->height_mm;
	r.x_inc_next = attribute->width_mm;

	r.y_min_mm = r.y_min_mm < r.y_mm - r.y_inc_next ? r.y_min_mm : r.y_mm - r.y_inc_next;
	r.x_max_mm = r.x_max_mm < r.x_mm + r.x_inc_next ? r.x_mm + r.x_inc_next : r.x_max_mm;


	return r;
}


int 
nhgui_icon_blank_initialize(struct nhgui_icon_blank_instance *instance)
{

	char vertex_source[] = 
				"#version 430 core \n "
				"layout(location=0) in vec2 v_position; \n"
				"uniform vec2 position; \n"
				"uniform vec2 size;	\n"
				"void main(){		\n"
				"	gl_Position = vec4(position + 2.0*(v_position/2.0 + 0.5)*size, 0.0, 1.0);	\n"
				"}		\n";

	char fragment_source[] = 
				"#version 430 core	\n"
				"uniform vec3 color;	\n"
				"out vec4 fcolor;	\n"
				"void main(){		\n"
				"	fcolor = vec4(color, 0);	\n"
				"}			\n";	

	instance->program = nhgui_shader_vertex_create_from_memory(
			vertex_source, sizeof(vertex_source), 
			fragment_source, sizeof(fragment_source)
	);

	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_memory() failed. \n");
		return -1;	
	
	}

	int result = nhgui_common_uniform_locations_find(&instance->locations, instance->program);
	if(result < 0){
		glDeleteProgram(instance->program);

		fprintf(stderr, "nhgui_common_uniform_locations_find() failed. \n");
		return -1;
	}


	return 0;
}

void
nhgui_icon_blank_deinitialize(struct nhgui_icon_blank_instance *instance)
{
	glDeleteProgram(instance->program);
}



struct nhgui_result
nhgui_icon_menu(
		struct nhgui_icon_menu *object,
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
)
{


	const struct nhgui_icon_menu_instance *instance = &context->menu;

	float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
	float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;
	
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

	nhgui_common_uniform_locations_set(
			&instance->locations, 
			context, input, result_tmp, 
			attribute->height_mm, attribute->height_mm,
			attribute->r, attribute->g, attribute->b
	);

	nhgui_surface_render(&context->surface);

	struct nhgui_result render_result = result;
	render_result.y_inc_next = attribute->height_mm;
	render_result.x_inc_next = attribute->height_mm;


	return render_result;
}


int 
nhgui_icon_menu_initialize(struct nhgui_icon_menu_instance *instance)
{
	char vertex_source[] = 
				"#version 430 core \n "
				"layout(location=0) in vec2 v_position; \n"
				"uniform vec2 position; \n"
				"uniform vec2 size;	\n"
				"void main(){		\n"
				"	gl_Position = vec4(position + 2.0*(v_position/2.0 + 0.5)*size, 0.0, 1.0);	\n"
				"}		\n";

	char fragment_source[] = 
				"#version 430 core	\n"
				"uniform vec3 color;		\n"
				"uniform uvec2 dimension;	\n"
				"uniform vec2 position;		\n"
				"uniform vec2 size;	\n"
				"out vec4 fcolor;	\n"
				"void main(){		\n"
				"	vec2 dim = vec2(dimension);			\n"
				"	vec2 uv = 2.0*vec2(gl_FragCoord.xy)/dim - 1.0;	\n"
				"	float y1 = position.y + 2.0*size.y*3.0/10.0;	\n"
				"	float y2 = position.y + 2.0*size.y*7.0/10.0;	\n"
				"	float s = 2.0*size.y*1.0/10.0;			\n"
				"	if((uv.y < y1 - s && uv.y < y1 + s) || (uv.y > y2 - s && uv.y < y2+s)){	\n"
				"		discard;	\n"
				"	}else{			\n"
				"		fcolor = vec4(color, 0);	\n"
				"	}				\n"
				"}			\n";	

	instance->program = nhgui_shader_vertex_create_from_memory(
			vertex_source, sizeof(vertex_source), 
			fragment_source, sizeof(fragment_source)
	);

	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_memory() failed. \n");
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

struct nhgui_result 
nhgui_icon_text_cursor(
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
)
{
	
	const struct nhgui_icon_text_cursor_instance *instance = &context->text_cursor;

	glUseProgram(instance->program);	
	
	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;
	nhgui_common_uniform_locations_set(
			&instance->locations,
		       	context, input, result_tmp, 
			attribute->width_mm, attribute->height_mm,
			attribute->r, attribute->g, attribute->b
	);

	/* Make the cursor bllink */
	uint32_t time_sec = (uint32_t)input->time_sec;
	float r = input->time_sec - time_sec;
	if(r < 0.5f)
		nhgui_surface_render(&context->surface);
	
	struct nhgui_result res = result;
	res.y_mm += attribute->height_mm;
	res.x_mm += attribute->height_mm;

	return res;
}


int 
nhgui_icon_text_cursor_initialize(struct nhgui_icon_text_cursor_instance *instance)
{
	char vertex_source[] = 
				"#version 430 core \n "
				"layout(location=0) in vec2 v_position; \n"
				"uniform vec2 position; \n"
				"uniform vec2 size;	\n"
				"void main(){		\n"
				"	gl_Position = vec4(position + 2.0*(v_position/2.0 + 0.5)*size, 0.0, 1.0);	\n"
				"}		\n";

	char fragment_source[] = 
				"#version 430 core	\n"
				"uniform vec3 color;	\n"
				"out vec4 fcolor;	\n"
				"void main(){		\n"
				"	fcolor = vec4(color, 0);	\n"
				"}			\n";	

	instance->program = nhgui_shader_vertex_create_from_memory(
			vertex_source, sizeof(vertex_source), 
			fragment_source, sizeof(fragment_source)
	);


	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_file() failed. \n");
		return -1;	
	}
	
	int result = nhgui_common_uniform_locations_find(
			&instance->locations, 
			instance->program
	);

	if(result < 0){
		glDeleteProgram(instance->program);

		fprintf(stderr, "nhgui_common_uniform_locations_find() failed. \n");
		return -1;
	}

	return 0;
}

void
nhgui_icon_text_cursor_deinitialize(struct nhgui_icon_text_cursor_instance *instance)
{
	glDeleteProgram(instance->program);
}

uint32_t 
nhgui_object_font_text_overflow_count(
		const struct nhgui_result within, 
		const struct nhgui_context *context,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text,
		const uint32_t text_length
)
{


	/* Find number of characters that is past result, 
	 * that is all characters that is outside of the 
	 * blank that is used as background. 
	 */
	float x_mm = 0.0f;
	float x_mm_max = within.x_mm + within.x_inc_next;
	uint32_t overflow_count = 0;
	for(uint32_t i = 0; i < text_length; i++)
	{
		unsigned char c = text[i];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		
		float new_x_mm = x_mm + (float)(font->character[c].advance_x >> 6) * mm_per_pixel_x;
		/* See if it is past the boudning box */
		if(within.x_mm + new_x_mm > x_mm_max)
		{
			overflow_count = text_length - i;		
			break;
		}
		x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
	}

	return overflow_count;
}
	
struct nhgui_result
nhgui_object_scroll_bar_scroll_result(
		const struct nhgui_object_scroll_bar *bar,
		const struct nhgui_result result
)
{
	struct nhgui_result r = result;
	r.y_offset_mm += bar->scroll_y_mm;
	r.x_offset_mm -= bar->scroll_x_mm;
	return r;
}

void
nhgui_object_scroll_bar(
		struct nhgui_object_scroll_bar *bar,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *scroll_attribute,
		const struct nhgui_render_attribute *size_attribute,
		struct nhgui_input *input, 
		const struct nhgui_result scroll_result,	
		const struct nhgui_result result	
)
{

	/* Compute the overflow in x and y direction and the % of overflow 
	 * compared to the size attribute corresponds to space that can be 
	 * scrolled.
	 *
	 * The y scroll bar consumed space in x direction so the x bar have 
	 * to be adjusted to be able to scroll past the space consumed by 
	 * the y scroll bar. 
	 * */
	
	float x_mm_used = 0.0f;
	float overflow_y_mm = (scroll_result.y_mm -  result.y_min_mm) - size_attribute->height_mm; 
	if(overflow_y_mm > 0.0f)
	{	
		/* Compute % that is overflowing */
		float y_mm_size = size_attribute->height_mm;
		float p = (y_mm_size-overflow_y_mm)/(y_mm_size);
		
		/* Scale scroll bar with the %. If to small make it height */
		float scroll_bar_size_mm = y_mm_size * p < scroll_attribute->height_mm ? scroll_attribute->height_mm : y_mm_size * p;
		struct nhgui_render_attribute blank_scroll_attribute = {
			.height_mm = scroll_bar_size_mm,
			.width_mm = scroll_attribute->width_mm,
			.r = scroll_attribute->r,
			.g = scroll_attribute->g,
			.b = scroll_attribute->b
		};
		
		/* size conusmed by the y scroll bar */
		x_mm_used = blank_scroll_attribute.width_mm;
		
		/* Place the scrollbar in the right of the scroll result */	
		struct nhgui_result scroll_result_y = scroll_result;
		scroll_result_y.x_mm = scroll_result.x_mm + size_attribute->width_mm - blank_scroll_attribute.width_mm;
		scroll_result_y.y_mm -= bar->scroll_y_mm;


		
		nhgui_icon_blank(
				&bar->blank_scroll_y,
				context, 
				&blank_scroll_attribute,
				input, 
				scroll_result_y
		);

		if(bar->blank_scroll_y.pressed > 0)
		{
			/* Allow moving the scroll bar */	
			float scroll_area_mm = y_mm_size - scroll_bar_size_mm;
			float mm_per_pixel = (float)context->screen_height_mm/(float)context->screen_resolution_y;
			float scroll_in_mm = input->cursor_y_delta_pixel * mm_per_pixel;
			float max_scroll = bar->scroll_y_mm + scroll_in_mm < scroll_area_mm ? scroll_in_mm : bar->scroll_y_mm + scroll_in_mm - scroll_area_mm;
			bar->scroll_y_mm = bar->scroll_y_mm-max_scroll > 0 ? bar->scroll_y_mm-max_scroll : 0;


		
		}
	}

	float x_mm_size = size_attribute->width_mm;
	float x_overflow_mm = result.x_max_mm + 2*x_mm_used - x_mm_size - scroll_result.x_mm;
	if(x_overflow_mm > 0.0)
	{
		float p = (x_mm_size-x_overflow_mm)/(x_mm_size);

		float scroll_bar_size_mm = x_mm_size * p < scroll_attribute->height_mm ? scroll_attribute->height_mm : x_mm_size * p;
		struct nhgui_render_attribute blank_scroll_attribute = {
			.height_mm = scroll_attribute->width_mm,
			.width_mm = scroll_bar_size_mm,
			.r = scroll_attribute->r,
			.g = scroll_attribute->g,
			.b = scroll_attribute->b
		};


		struct nhgui_result scroll_result_x = {
			.x_mm = scroll_result.x_mm + bar->scroll_x_mm,
			.y_mm = scroll_result.y_mm + scroll_attribute->width_mm - size_attribute->height_mm,
		};

		
		nhgui_icon_blank(
				&bar->blank_scroll_x,
				context, 
				&blank_scroll_attribute,
				input, 
				scroll_result_x	
		);
		if(bar->blank_scroll_x.pressed > 0)
		{
			float scroll_area_mm = x_mm_size - scroll_bar_size_mm - x_mm_used;
			float mm_per_pixel = (float)context->screen_width_mm/(float)context->screen_resolution_x;
			float scroll_in_mm = input->cursor_x_delta_pixel * mm_per_pixel;
			float max_scroll = bar->scroll_x_mm + scroll_in_mm < 0 ? 0 : scroll_in_mm;
			bar->scroll_x_mm = bar->scroll_x_mm+max_scroll < scroll_area_mm ? bar->scroll_x_mm+max_scroll : scroll_area_mm;
		}
		else
		{
			float scroll_area_mm = x_mm_size - scroll_bar_size_mm - x_mm_used;
			bar->scroll_x_mm = bar->scroll_x_mm < scroll_area_mm ? bar->scroll_x_mm : scroll_area_mm;
		}

	}
	else
	{
		bar->scroll_x_mm = 0;
	}
	


}


struct nhgui_result
nhgui_window_begin(
		struct nhgui_window *window,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input,
		const struct nhgui_result result
)
{
	/* Store the result such that the window initial positions are known */
	window->result_begin = result;
	
	/* Compute the screen coordinates of the window and only alllow drawing 
	 * to the pixels within the area using scissor test in opengl. */	
	float x_pixels_per_mm = (float)context->screen_resolution_x/(float)context->screen_width_mm;
	float y_pixels_per_mm = (float)context->screen_resolution_y/(float)context->screen_height_mm;
	
	uint32_t x = result.x_mm * x_pixels_per_mm;
	uint32_t y = (result.y_mm - attribute->height_mm)*y_pixels_per_mm < 0 ? 0 : (result.y_mm - attribute->height_mm)*y_pixels_per_mm; 
	uint32_t height_remove_mm = result.y_mm - attribute->height_mm < 0 ? -(result.y_mm - attribute->height_mm) : 0;
	uint32_t width = attribute->width_mm * x_pixels_per_mm;
	uint32_t height = (attribute->height_mm - height_remove_mm) * y_pixels_per_mm;

	glEnable(GL_SCISSOR_TEST);
	glScissor(
		x,
		y,
		width,
		height	
	);


	/* Apply offsets that are adjusted by the scroll bars to the result. */
	return nhgui_object_scroll_bar_scroll_result(
			&window->scroll_bar,
			result
	);

}

struct nhgui_result 
nhgui_window_end(
		struct nhgui_window *window,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input,
		const struct nhgui_result result
)
{
	
	struct nhgui_result window_result = window->result_begin;

	glDisable(GL_SCISSOR_TEST);

	window_result.x_inc_next = attribute->width_mm;
	window_result.y_inc_next = attribute->height_mm;

	window_result.y_min_mm = result.y_min_mm < result.y_mm + result.y_inc_next ? result.y_min_mm : result.y_mm + result.y_inc_next;
	window_result.x_max_mm = result.x_max_mm < result.x_mm + result.x_inc_next ? result.x_mm + result.x_inc_next : result.x_max_mm;

	window_result.x_offset_mm = 0;
	window_result.y_offset_mm = 0;

	nhgui_object_scroll_bar(
			&window->scroll_bar,
			context,
			&window->scroll_bar_attribute, 
			attribute,
			input,
			window->result_begin,
			result
	);


	return window_result;

}
			


struct nhgui_result
nhgui_object_text_list(
		struct nhgui_object_text_list *list,
		const struct nhgui_context *context,
		const char *entry[],
		const uint32_t *entry_length,
		const uint32_t entry_count,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		const struct nhgui_result result

)
{

	float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
	float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;


	struct nhgui_result r = result;	
	for(uint32_t i = 0; i < entry_count; i++)
	{
		
		struct nhgui_result result_tmp = r;
		result_tmp.y_mm -= attribute->height_mm;
		
		if(input->cursor_button_left > 0)
		{	
			if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
			&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
			{
				if(list->selected_index == i)
					list->selected = list->selected ? 0 : 1;
				else
					list->selected = 1;


				list->selected_index = i;

				if(list->selected > 0)
				{
					input->selected_new_raise = 1;	
				}	
			}	
		}

		if(list->selected > 0 && list->selected_index == i)
		{
		
			struct nhgui_render_attribute selected_attribute = {
				.height_mm = attribute->height_mm,
				.width_mm = attribute->width_mm,
				.r = list->selected_field_color.x,
				.g = list->selected_field_color.y,
				.b = list->selected_field_color.z,
			};

			r = nhgui_icon_blank_no_object(
					context,
					&selected_attribute,
					input,
					r	
			);
			
			list->selected_result = r;

			uint32_t overflow_count_index = 0;
			uint32_t overflow_count = nhgui_object_font_text_overflow_count(
				r,	
				context,
				font,
				attribute,
				entry[i],
				entry_length[i]
			);
			
			
			if(overflow_count > 0 && list->char_scroll_per_sec > 0)
			{
				uint32_t s = input->time_sec/list->char_scroll_per_sec;
				overflow_count_index = s%overflow_count;
			}

			struct nhgui_render_attribute selected_font_attribute = *attribute;
			selected_font_attribute.r = list->selected_text_color.x;
			selected_font_attribute.g = list->selected_text_color.y;
			selected_font_attribute.b = list->selected_text_color.z;

			nhgui_object_font_text(
					context, 
					font, 
					&entry[i][overflow_count_index],
					entry_length[i] - overflow_count_index,
					&selected_font_attribute,
					input, 
					r	
			);
		}
		else
		{

			struct nhgui_render_attribute _attribute = {
				.height_mm = attribute->height_mm,	
				.width_mm = attribute->width_mm,
				.r = list->field_color.x,
				.g = list->field_color.y,
				.b = list->field_color.z,
			};


			r = nhgui_icon_blank_no_object(
					context,
					&_attribute,
					input,
					r	
			);


			uint32_t overflow_count_index = 0;
			uint32_t overflow_count = nhgui_object_font_text_overflow_count(
				r,	
				context,
				font,
				attribute,
				entry[i],
				entry_length[i]
			);

			if(overflow_count > 0 && list->char_scroll_per_sec > 0)
			{
				uint32_t s = input->time_sec/list->char_scroll_per_sec;
				overflow_count_index = (overflow_count + s)%overflow_count;
			}

			struct nhgui_render_attribute font_attribute = *attribute;
			font_attribute.r = list->text_color.x;
			font_attribute.g = list->text_color.y;
			font_attribute.b = list->text_color.z;

			nhgui_object_font_text(
					context, 
					font, 
					&entry[i][overflow_count_index],
					entry_length[i] - overflow_count,
					&font_attribute,
					input, 
					r	
			);


		}

		if(i != entry_count - 1)
			r = nhgui_result_dec_y(r);

	}

	return r;
}



struct nhgui_result 
nhgui_object_input_field(
		struct nhgui_object_input_field *field,
		const struct nhgui_context *context,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		const struct nhgui_result result,
		char *input_buffer, 
		uint32_t *input_buffer_length,
		const uint32_t input_buffer_size
)
{

	
	

	struct nhgui_render_attribute blank_attribute = 
	{
		.width_mm = attribute->width_mm,
		.height_mm = attribute->height_mm,	
		.r = field->field_color.x,
		.g = field->field_color.y,
		.b = field->field_color.z,
	};

	/* Background of the input field.
	 * Used for input too.  */
	struct nhgui_result background_result = nhgui_icon_blank(
			&field->blank_object,
			context,
			&blank_attribute,
			input,
			result
	);

	/* If the blank object, that is the background of 
	 * the input field is selected. Then process input 
	 * from the keyboard */	
	if(field->blank_object.selected > 0)
	{
		int32_t delta_characters = nhgui_input_buffer(
				input_buffer, 
				input_buffer_length,
				input_buffer_size, 
				input, 
				&field->cursor_index	
		);

		(void)delta_characters; 
	}


	/* Find number of characters that is past result, 
	 * that is all characters that is outside of the 
	 * blank that is used as background. 
	 */
	
	float x_mm = 0.0f;
	float x_mm_max = background_result.x_mm + background_result.x_inc_next;
	uint32_t overflow_count = 0;
	for(uint32_t i = 0; i < *input_buffer_length; i++)
	{
		unsigned char c = input_buffer[i];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		
		float cursor_mm = attribute->height_mm;
		float new_x_mm = x_mm + (float)(font->character[c].advance_x >> 6) * mm_per_pixel_x + cursor_mm;
		/* See if it is past the boudning box */
		if(background_result.x_mm + new_x_mm > x_mm_max)
		{
			overflow_count = *input_buffer_length - i;		
			break;
		}
		x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
	}

	
	/* If the blank was clicked then compute the index of the selcted character, 
	 * if no index is found. Then it is safe to assume that area clicked was 
	 * past the text and cursor index is set as the last character */
	struct nhgui_result cursor_result = background_result ;
	if(field->blank_object.clicked > 0)
	{
		float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
		float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;
		
		uint32_t index_found = 0;
		for(uint32_t i = overflow_count; i < *input_buffer_length; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
			float x_mm_inc = (font->character[c].advance_x >> 6) * mm_per_pixel_x;
			if(cursor_x_mm > cursor_result.x_mm && cursor_x_mm < cursor_result.x_mm + x_mm_inc
			&& cursor_y_mm > cursor_result.y_mm - attribute->height_mm && cursor_y_mm < cursor_result.y_mm)
			{
				field->cursor_index = i;
				index_found = 1;
				break;	
			}
	
			cursor_result.x_mm += x_mm_inc;
		}

		if(index_found == 0)	
		{
			field->cursor_index = *input_buffer_length;		
		}
	}
	else
	{
		for(uint32_t i = overflow_count; i < field->cursor_index; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
			float x_mm_inc = (font->character[c].advance_x >> 6) * mm_per_pixel_x;
			cursor_result.x_mm += x_mm_inc;
		}
	
	}

	/* Find where the cursor should be placed */
	struct nhgui_render_attribute cursor_attribute = 
	{
		.width_mm = attribute->height_mm,
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};

	if(field->cursor_index < *input_buffer_length)
	{
		/* Cursor is before last character. Make it the 
		 * same size as the character it is hovering. */

		/* Find width of character at cursor index. */
		unsigned char c = input_buffer[field->cursor_index];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		float cursor_width_mm = font->character[c].width * mm_per_pixel_x;
		
		cursor_attribute.width_mm = cursor_width_mm;

	}
	else
	{
		/* Cursor is past last character */	
	
	}
		
		
	if(field->blank_object.selected > 0)
	{	
		/* This is cursor placed behind the text */
		nhgui_icon_text_cursor(
				context,
				&cursor_attribute,
				input,
				cursor_result	
		);
	}

	struct nhgui_render_attribute font_attribute = 
	{
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};


	nhgui_object_font_text(
			context, 
			font, 
			&input_buffer[overflow_count],
			*input_buffer_length - overflow_count,
			&font_attribute,
			input, 
			result
	);
	
	return background_result;

}



struct nhgui_result 
nhgui_object_input_field_float(
		struct nhgui_object_input_field_float *float_field,
		const struct nhgui_context *context,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		const struct nhgui_result result,
		float *value
		
)
{
	struct nhgui_object_input_field *field = &float_field->field;

	struct nhgui_render_attribute blank_attribute = 
	{
		.width_mm = attribute->width_mm,
		.height_mm = attribute->height_mm,	
		.r = field->field_color.x,
		.g = field->field_color.y,
		.b = field->field_color.z,
	};

	/* Background of the input field.
	 * Used for input too.  */
	struct nhgui_result background_result = nhgui_icon_blank(
			&field->blank_object,
			context,
			&blank_attribute,
			input,
			result
	);


	char *input_buffer = float_field->str;
	uint32_t *input_buffer_length = &float_field->str_length;
	uint32_t input_buffer_size = sizeof(float_field->str);
	
	/* Dont update the value if it is selected */
	if(field->blank_object.selected > 0){}
	/* If selected previously, then update the float value */	
	else if(field->blank_object.deselected > 0)
	{
		printf("Updated value \n");
		*value = atof(float_field->str);
	}
	else
	{
		float_field->str_length = snprintf(float_field->str, sizeof(float_field->str), "%f", *value);	
	}
#if 0
	if(float_field->str_initialized == 0)
	{
		float_field->str_length = snprintf(float_field->str, sizeof(float_field->str), "%f", *value);	
		float_field->str_initialized = 1;

	}
#endif 
		
	/* If the blank object, that is the background of 
	 * the input field is selected. Then process input 
	 * from the keyboard */	
	if(field->blank_object.selected > 0)
	{
		int32_t delta_characters = nhgui_input_buffer(
				input_buffer, 
				input_buffer_length,
				input_buffer_size, 
				input, 
				&field->cursor_index	
		);

		(void)delta_characters; 
	}

	const char single = '.';
	const char front = '-';
	const char allowed[] = "1234567890.-";

	for(uint32_t i = 1; i < input_buffer_size; i++)
	{
		if(front == float_field->str[i])	
		{
			uint32_t count = sizeof(float_field->str) - i - 1;
			memcpy(&float_field->str[i], &float_field->str[i+1], count);	

			if(*input_buffer_length > 0)
				*input_buffer_length -= 1;
			if(field->cursor_index > 0)
				field->cursor_index -= 1;
		}
	}
			
	uint32_t single_found = 0;
	for(uint32_t i = 0; i < input_buffer_size; i++){
		if(single == float_field->str[i] && single_found > 0)
		{
			uint32_t count = sizeof(float_field->str) - i - 1;
			memcpy(&float_field->str[i], &float_field->str[i+1], count);	

			if(*input_buffer_length > 0)
				*input_buffer_length -= 1;
			if(field->cursor_index > 0)
				field->cursor_index -= 1;
		}		
		else if(single == float_field->str[i])
		{
			single_found = 1;	
		}
	}
	
	for(uint32_t i = 0; i < sizeof(float_field->str); i++)
	{
		uint32_t allowed_found = 0;
		for(uint32_t j = 0; j < sizeof(allowed); j++)
		{
			if(float_field->str[i] == allowed[j]){
				allowed_found = 1;
				break;
			}	
		}

		if(allowed_found == 0)
		{
			uint32_t count = sizeof(float_field->str) - i - 1;
			memcpy(&float_field->str[i], &float_field->str[i+1], count);	
			
			if(*input_buffer_length > 0)
				*input_buffer_length -= 1;
			if(field->cursor_index > 0)
				field->cursor_index -= 1;
		}
		
	}	

	/* If the input field is selected, then dont update the value such that
	 * it can be edited. 
	 */
	if(field->blank_object.selected > 0)
	{
	
	}
	else 
	{
		*value = atof(float_field->str);
	}



	/* Find number of characters that is past result, 
	 * that is all characters that is outside of the 
	 * blank that is used as background. 
	 */
	
	float x_mm = 0.0f;
	float x_mm_max = background_result.x_mm + background_result.x_inc_next;
	uint32_t overflow_count = 0;
	for(uint32_t i = 0; i < *input_buffer_length; i++)
	{
		unsigned char c = input_buffer[i];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		
		float cursor_mm = attribute->height_mm;
		float new_x_mm = x_mm + (float)(font->character[c].advance_x >> 6) * mm_per_pixel_x + cursor_mm;
		/* See if it is past the boudning box */
		if(background_result.x_mm + new_x_mm > x_mm_max)
		{
			overflow_count = *input_buffer_length - i;		
			break;
		}
		x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
	}

	
	/* If the blank was clicked then compute the index of the selcted character, 
	 * if no index is found. Then it is safe to assume that area clicked was 
	 * past the text and cursor index is set as the last character */
	struct nhgui_result cursor_result = background_result ;
	if(field->blank_object.clicked > 0)
	{
		float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
		float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;
		
		uint32_t index_found = 0;
		for(uint32_t i = overflow_count; i < *input_buffer_length; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
			float x_mm_inc = (font->character[c].advance_x >> 6) * mm_per_pixel_x;
			if(cursor_x_mm > cursor_result.x_mm && cursor_x_mm < cursor_result.x_mm + x_mm_inc
			&& cursor_y_mm > cursor_result.y_mm - attribute->height_mm && cursor_y_mm < cursor_result.y_mm)
			{
				field->cursor_index = i;
				index_found = 1;
				break;	
			}
	
			cursor_result.x_mm += x_mm_inc;
		}

		if(index_found == 0)	
		{
			field->cursor_index = *input_buffer_length;		
		}
	}
	else
	{
		for(uint32_t i = overflow_count; i < field->cursor_index; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
			float x_mm_inc = (font->character[c].advance_x >> 6) * mm_per_pixel_x;
			cursor_result.x_mm += x_mm_inc;
		}
	}

	/* Find where the cursor should be placed */
	struct nhgui_render_attribute cursor_attribute = 
	{
		.width_mm = attribute->height_mm,
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};

	if(field->cursor_index < *input_buffer_length)
	{
		/* Cursor is before last character. Make it the 
		 * same size as the character it is hovering. */

		/* Find width of character at cursor index. */
		unsigned char c = input_buffer[field->cursor_index];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		float cursor_width_mm = font->character[c].width * mm_per_pixel_x;
		
		cursor_attribute.width_mm = cursor_width_mm;

	}
	else
	{
		/* Cursor is past last character */	
	
	}
		
		
	if(field->blank_object.selected > 0)
	{	
		/* This is cursor placed behind the text */
		nhgui_icon_text_cursor(
				context,
				&cursor_attribute,
				input,
				cursor_result	
		);
	}

	struct nhgui_render_attribute font_attribute = 
	{
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};


	nhgui_object_font_text(
			context, 
			font, 
			&input_buffer[overflow_count],
			*input_buffer_length - overflow_count,
			&font_attribute,
			input, 
			result
	);
	return background_result;
}

int 
nhgui_object_font_freetype_initialize(struct nhgui_object_font_freetype *freetype)
{
	if(FT_Init_FreeType(&freetype->ft) != 0)
	{
		fprintf(stderr, "FT_Init_Freetype() failed. \n");
		return -1;	
	}

	return 0;
}

void 
nhgui_object_font_freetype_deinitialize(struct nhgui_object_font_freetype *freetype)
{
	FT_Done_FreeType(freetype->ft);
}

int 
nhgui_object_font_freetype_characters_initialize(
		struct nhgui_object_font_freetype *freetype,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
	       	struct nhgui_object_font *font,
	       	const char *filename
)
{
	FT_Face face;
	if(FT_New_Face(freetype->ft, filename, 0, &face))
	{
		fprintf(stderr, "FT_New_Face() failed. \n");
		return -1;
	}
	
	/* Generate characters with specified height. The width is auto 
	 * scaled depending on chracter */	
	uint32_t pixels_per_mm = context->screen_resolution_y/context->screen_height_mm;
	FT_Set_Pixel_Sizes(face, 0, attribute->height_mm * pixels_per_mm);

	font->height_mm = attribute->height_mm;
	
	/* Allocate chunck for easier clenup in case of error */	
	GLuint texture[128];
	glGenTextures(128, texture);
	
	/* Load ansi characters */
	for(uint32_t i = 0; i < 128; i++)
	{
		if(FT_Load_Char(face, i, FT_LOAD_RENDER)){
			fprintf(stderr, "Could not load characters %c from font file. \n", i);
			glDeleteTextures(128, texture);
			return -1;	
		}

		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
			       	face->glyph->bitmap.width, face->glyph->bitmap.rows,
			       	0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		font->character[i].texture = texture[i];
		font->character[i].width = face->glyph->bitmap.width;
		font->character[i].height = face->glyph->bitmap.rows;
		font->character[i].bearing_x = face->glyph->bitmap_left;
		font->character[i].bearing_y = face->glyph->bitmap_top;
		font->character[i].advance_x = face->glyph->advance.x;

	}
	
	FT_Done_Face(face);

	const char text[] = "abcdefghlm";
	font->delta_y_max = nhgui_object_font_text_delta_y_max(
			context,
			font,
			attribute,
			text,
			sizeof(text)
	);

	return 0;	
}

void 
nhgui_object_font_freetype_characters_deinitialize(
	       	struct nhgui_object_font *font
)
{
	for(uint32_t i = 0; i < 128; i++)
	{
		glDeleteTextures(1, &font->character[i].texture);		
	}	
}

void
nhgui_object_font_text_deinitialize(struct nhgui_object_font_text_instance *instance)
{
	glDeleteProgram(instance->program);
}

int 
nhgui_object_font_text_initialize(struct nhgui_object_font_text_instance *instance)
{
	char vertex_source[] = 
				"#version 430 core \n "
				"layout(location=0) in vec2 v_position; \n"
				"uniform vec2 position; \n"
				"uniform vec2 size;	\n"
				"out vec2 fragcoord;	\n"
				"void main(){		\n"
				"	fragcoord.x = (v_position + vec2(1)).x/2;		\n"
				"	fragcoord.y = 1 - (v_position + vec2(1)).y/2;		\n"
				"	gl_Position = vec4(position + 2.0*(v_position/2.0 + 0.5)*size, 0.0, 1.0);	\n"
				"}		\n";

	char fragment_source[] = 
				"#version 430 core	\n"
				"uniform vec3 color;	\n"
				"uniform sampler2D in_texture;	\n"
				"in vec2 fragcoord;		\n"
				"out vec4 fcolor;	\n"
				"void main(){		\n"
				"	vec4 s = vec4(1,1,1, texture(in_texture, fragcoord).r);	\n"
				"	fcolor = vec4(color, 1.0) * s;	\n"
				"}			\n";	

	instance->program = nhgui_shader_vertex_create_from_memory(
			vertex_source, sizeof(vertex_source), 
			fragment_source, sizeof(fragment_source)
	);	
	
	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_file() failed. \n");
		return -1;	
	}
	
	int result = nhgui_common_uniform_locations_find(
			&instance->locations, 
			instance->program
	);

	if(result < 0){
		glDeleteProgram(instance->program);
		
		fprintf(stderr, "nhgui_common_uniform_locations_find() failed. \n");
		return -1;
	}

	return 0;
}


struct nhgui_result
nhgui_object_font_text_result_centered_by_previous_x(
		const struct nhgui_result result,
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text,
		const uint32_t text_length
)
{
	/* Compute the length of the text string and center it with regards 
	 * to the previous element size */

	float x_mm = 0.0f;
	for(uint32_t i = 0; i < text_length; i++)
	{
		unsigned char c = text[i];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;

		if(i != text_length-1)
			x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
	}
	struct nhgui_result r = result;
	r.x_mm += result.x_inc_next/2 - x_mm/2;
	return r;
}


float 
nhgui_object_font_text_delta_y_max(
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text, 
		const uint32_t text_length
)
{
	float delta_y_max = 0;	
	for(uint32_t i = 0; i < text_length; i++)
	{	
		unsigned char c = (unsigned char)text[i];
		float ratio = attribute->height_mm/font->height_mm;
		float mm_per_pixel_y = ratio * (float)context->screen_height_mm/(float)context->screen_resolution_y;

		float delta  = font->character[c].height * mm_per_pixel_y;
		
		delta_y_max = delta_y_max < delta ? delta : delta_y_max;
	}

	return delta_y_max;
}

struct nhgui_result
nhgui_object_font_text(
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const char *text, 
		const uint32_t text_length, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
)
{
	/* Calculate the maximum delta in y such that the text can be centered in
	 * the rendering loop.
	 **/	

	const struct nhgui_object_font_text_instance *instance = &context->font;

	float ratio = attribute->height_mm/font->height_mm;

	float delta_y_max = ratio * font->delta_y_max;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(instance->program);

	struct nhgui_result render_result = result;
	struct nhgui_result r = result;

	for(uint32_t i = 0; i < text_length; i++)
	{	
		unsigned char c = (unsigned char)text[i];


		float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
		float mm_per_pixel_y = ratio * (float)context->screen_height_mm/(float)context->screen_resolution_y;
		
		float width_mm = (float)font->character[c].width * mm_per_pixel_x;
		float height_mm = (float)font->character[c].height * mm_per_pixel_y;
		
		struct nhgui_result result_tmp = result;	
		result_tmp.x_mm = r.x_mm + font->character[c].bearing_x * mm_per_pixel_x;
		result_tmp.y_mm = r.y_mm - (font->character[c].height - font->character[c].bearing_y) * mm_per_pixel_y - delta_y_max; 

		nhgui_common_uniform_locations_set(
				&instance->locations,
			       	context, input, result_tmp, 
				width_mm, height_mm,
				attribute->r, attribute->g, attribute->b
		);

		r.x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, font->character[c].texture);

		nhgui_surface_render(&context->surface);
	}

	render_result.x_inc_next = r.x_mm;
	render_result.y_inc_next = delta_y_max;

	glDisable(GL_BLEND);

	return render_result;
}

struct nhgui_result 
nhgui_object_font_text_area(
		const struct nhgui_object_font_text_area *area,
		const struct nhgui_context *context,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result,
		const char *input_buffer, 
		const uint32_t input_buffer_size
)

{
	struct nhgui_result background_result = result; 
	uint32_t curr_char = 0;

	while(curr_char < input_buffer_size)
	{
			
		struct nhgui_render_attribute blank_attribute = 
		{
			.width_mm = attribute->width_mm,
			.height_mm = attribute->height_mm,	
			.r = area->background_color.x,
			.g = area->background_color.y,
			.b = area->background_color.z,
		};

		/* Background of the input field. */
		background_result = nhgui_icon_blank_no_object(
				context,
				&blank_attribute,
				input,
				background_result	
		);


		
		float x_mm = 0.0f;
		float x_mm_max = background_result.x_mm + background_result.x_inc_next;

		/* Make sure that curr_char always is increased by one */
		uint32_t char_within = 1;
		for(uint32_t i = curr_char; i < input_buffer_size; i++)
		{
			unsigned char c = input_buffer[i];
			float ratio = attribute->height_mm/font->height_mm;
			float mm_per_pixel_x = ratio * (float)context->screen_width_mm/(float)context->screen_resolution_x;
			
			float new_x_mm = x_mm + (float)(font->character[c].advance_x >> 6) * mm_per_pixel_x;
			
		

			/* See if it is past the boudning box */
			if(background_result.x_mm + new_x_mm > x_mm_max)
			{
				break;
			}

			char_within += 1;

			x_mm += (font->character[c].advance_x >> 6) * mm_per_pixel_x;
		}

		/* Remove the character that was there just in case */
		if(char_within > 1){
			char_within -= 1;	
		}
		
	
		struct nhgui_render_attribute font_attribute = 
		{
			.height_mm = attribute->height_mm,	
			.r = area->font_color.x,
			.g = area->font_color.y,
			.b = area->font_color.z,
		};


		nhgui_object_font_text(
			context, 
			font, 
			&input_buffer[curr_char],
			char_within,
			&font_attribute,
			input, 
			background_result
		);

		curr_char += char_within;

		background_result = nhgui_result_dec_y(background_result);
	
	}

	return background_result;
}



struct nhgui_result
nhgui_object_radio_button(
	       	struct nhgui_object_radio_button *object,
		const struct nhgui_context *context,
	       	const struct nhgui_render_attribute *attribute,
	       	const struct nhgui_input *input,
	       	const struct nhgui_result result
)
{	

	const struct nhgui_object_radio_button_instance *instance = &context->radio_button;
	/* Calculate coordinates of the cursor in mm.
	 * First crate a scale relative to how large the application window is in comparison to the scrren size 
	 * then calculate pixels per mm  and mutiply by cursor position.
	 * */

	float cursor_x_mm = (float)input->width_pixel / (float)context->screen_resolution_x * (float)context->screen_width_mm/(float)input->width_pixel * (float)input->cursor_x_pixel;
	float cursor_y_mm = (float)input->height_pixel / (float)context->screen_resolution_y * (float)context->screen_height_mm/(float)input->height_pixel * (float)input->cursor_y_pixel;

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
	

	glUseProgram(instance->shader_program);	
	

	nhgui_common_uniform_locations_set(
			&instance->locations,
		       	context,
		       	input,
		       	result,
		       	attribute->width_mm, attribute->width_mm,
			attribute->r, attribute->g, attribute->b
	);


	nhgui_surface_render(&context->surface);

	struct nhgui_result r = result;
	r.y_inc_next = attribute->width_mm;
	r.x_inc_next = attribute->width_mm;

	r.y_min_mm = r.y_min_mm < r.y_mm - r.y_inc_next ? r.y_min_mm : r.y_mm - r.y_inc_next;
	r.x_max_mm = r.x_max_mm < r.x_mm + r.x_inc_next ? r.x_mm + r.x_inc_next : r.x_max_mm;


	return r;
}

int nhgui_object_radio_button_initialize(struct nhgui_object_radio_button_instance *instance)
{
	char vertex_source[] = 
				"#version 430 core \n "
				"layout(location=0) in vec2 v_position; \n"
				"uniform vec2 position; \n"
				"uniform vec2 size;	\n"
				"out float radius;	\n"
				"out vec2 center;	\n"
				"void main(){		\n"
				"	radius = size.x;	\n"
				"	center = position + size;	\n"
				"	gl_Position = vec4(position + 2.0*(v_position/2.0 + 0.5)*size, 0.0, 1.0);	\n"
				"}		\n";

	char fragment_source[] = 
				"#version 430 core	\n"
				"uniform vec3 color;	\n"
				"uniform uint checked;	\n"
				"uniform uvec2 dimension;	\n"
				"in vec2 center;	\n"
				"in float radius;	\n"
				"out vec4 fcolor;	\n"
				"void main(){		\n"
				"	vec2 dim = vec2(dimension);	\n"
				"	vec2 uv = 2.0*vec2(gl_FragCoord.xy) - 1.0;	\n"
				"	vec2 d = uv - center;				\n"
				"	d.x *= dim.x/dim.y;				\n"
				"	float r = radius * dim.x/dim.y;			\n"
				"	float v = dot(d, d);				\n"
				"	if(v < r*r){					\n"
				"		if(v < r * r - r*r/2){				\n"
				"			if(v < r*r - r*r*1.5/2 && checked > 0){	\n"
				"				fcolor = vec4(0, 0, 0, 0);	\n"
				"			}else{					\n"
				"				discard;"
				"			}		\n"
				"		}		\n"
				"	}				\n"
				"	else discard;		\n"
				"}			\n";	

	GLuint program = nhgui_shader_vertex_create_from_memory(
			vertex_source, sizeof(vertex_source), 
			fragment_source, sizeof(fragment_source)
	);

	if(program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_memory() failed. \n");
		return -1;	
	
	}



	instance->shader_program = program;


	int result = nhgui_common_uniform_locations_find(&instance->locations, instance->shader_program);
	if(result < 0){
		glDeleteProgram(instance->shader_program);

		fprintf(stderr, "nhgui_common_uniform_locations_find() failed. \n");
		return -1;
	}


	return 0;
}

void nhgui_object_radio_button_deinitialize(struct nhgui_object_radio_button_instance *instance)
{
	glDeleteProgram(instance->shader_program);
}

int32_t 
nhgui_input_buffer(
	char *input_buffer, 
	uint32_t *input_buffer_length,
	uint32_t input_buffer_size, 
	struct nhgui_input *input, 
	uint32_t *input_index
)
{
	int32_t delta = 0;

	/* In case a new buffer is provided and the previous buffer */
	if(*input_index > *input_buffer_length)
	{
		*input_index = *input_buffer_length;
	}

	/* If backspace, then remove characters */
	/* Only remove if there are something in the buffer */
	if(input->key_backspace_state  > 0 && *input_buffer_length > 0)
	{
		/* Remove last character */
		if(*input_index == *input_buffer_length)
		{
			*input_buffer_length -= 1;
			*input_index -= 1;	

			delta = -1;
		}
		/* Cant remove if cursor is at first character */
		else if(*input_index > 0)
		{

			/* Remove at cursor index */
			uint32_t count  = *input_buffer_length - *input_index;
			memcpy(&input_buffer[*input_index-1], &input_buffer[*input_index], count);	

			*input_buffer_length -= 1;
			*input_index -= 1;	
			delta = -1;
		}
	}
	/* If not backspace and there are input in the buffer */
	else if(input->input_length > 0)
	{
		/* Simply add to the last character the last character */
		if(*input_index == *input_buffer_length)
		{
			uint32_t space = input_buffer_size - *input_buffer_length; 
			uint32_t count = space < input->input_length ? space : input->input_length;
			memcpy(&input_buffer[*input_buffer_length], input->input, count);

			*input_buffer_length += count;
			*input_index += count;

			delta = count;

		}
		/* Make sure that there are space in the input buffer */
		else if(*input_buffer_length + input->input_length -1 < input_buffer_size)
		{
			/* Index to move everything to */
			uint32_t new_index = *input_index + input->input_length;
			/* Number of bytes to be moved */
			uint32_t copy_length = *input_buffer_length - new_index;	

			/* Copy chracter at cursor forward such that there is space for new chracter at the 
			 * position */
			memcpy(&input_buffer[new_index], &input_buffer[*input_index], copy_length); 

			/* Then copy new character into the free position */
			memcpy(&input_buffer[*input_index], input->input, input->input_length); 
		
			*input_buffer_length += input->input_length;
			*input_index += input->input_length;;

			delta = input->input_length;
		}
	}	
	return delta;
}




