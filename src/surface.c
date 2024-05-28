#include "surface.h"	


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


void nhgui_surface_render(struct nhgui_surface *nhgui_surface)
{
	glBindVertexArray(nhgui_surface->vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nhgui_surface->element_buffer);
	glDrawElements(GL_TRIANGLE_STRIP, nhgui_surface->draw_count, GL_UNSIGNED_INT, 0);	
	glBindVertexArray(0);

}

void nhgui_surface_render_instanced(struct nhgui_surface *nhgui_surface, uint32_t instance_count)
{
	glBindVertexArray(nhgui_surface->vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nhgui_surface->element_buffer);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, nhgui_surface->draw_count, GL_UNSIGNED_INT, 0, instance_count);	
	glBindVertexArray(0);
}



