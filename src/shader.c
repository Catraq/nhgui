#include "shader.h"



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


