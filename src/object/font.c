#include "object/font.h"

int 
nhgui_font_freetype_initialize(struct nhgui_font_freetype *freetype)
{
	if(FT_Init_FreeType(&freetype->ft) != 0)
	{
		fprintf(stderr, "FT_Init_Freetype() failed. \n");
		return -1;	
	}

	return 0;
}

void 
nhgui_font_freetype_deinitialize(struct nhgui_font_freetype *freetype)
{
	FT_Done_FreeType(freetype->ft);
}

int 
nhgui_font_freetype_characters_initialize(
		struct nhgui_font_freetype *freetype,
		struct nhgui_context *context,
		struct nhgui_render_attribute *attribute,
	       	struct nhgui_font_character character[128], 
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
	uint32_t pixels_per_mm = context->res_y/context->height_mm;
	FT_Set_Pixel_Sizes(face, 0, attribute->height_mm * pixels_per_mm);
	
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

		character[i].texture = texture[i];
		character[i].width = face->glyph->bitmap.width;
		character[i].height = face->glyph->bitmap.rows;
		character[i].bearing_x = face->glyph->bitmap_left;
		character[i].bearing_y = face->glyph->bitmap_top;
		character[i].advance_x = face->glyph->advance.x;
	}
	
	FT_Done_Face(face);

	return 0;	
}

void 
nhgui_font_freetype_characters_deinitialize(
	       	struct nhgui_font_character character[128]
)
{
	for(uint32_t i = 0; i < 128; i++)
	{
		glDeleteTextures(1, &character[i].texture);		
	}	
}

void
nhgui_font_text_deinitialize(struct nhgui_font_text_instance *instance)
{
	glDeleteProgram(instance->program);
}

int 
nhgui_font_text_initialize(struct nhgui_font_text_instance *instance)
{
	const char *vertex_source_filename = "../shaders/text.vs";
	const char *fragment_source_filename = "../shaders/text.fs";

	instance->program = nhgui_shader_vertex_create_from_file(
			vertex_source_filename,
			fragment_source_filename
	);

	if(instance->program == 0)
	{
		fprintf(stderr, "nhgui_shader_vertex_create_from_file() failed. \n");
		return -1;	
	}

			
	const char *position_uniform_str = "position";
	GLint position_location = glGetUniformLocation(instance->program, position_uniform_str);
	if(position_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", position_uniform_str);
		glDeleteProgram(instance->program);
		return -1;	
	}

	const char *size_uniform_str = "size";
	GLint size_location = glGetUniformLocation(instance->program, size_uniform_str);
	if(size_location == -1)
	{
		fprintf(stderr, "Could not find uniform location %s. \n", size_uniform_str);
		
		glDeleteProgram(instance->program);
		return -1;	
	}


	instance->locations.position = position_location;
	instance->locations.size = size_location;



	return 0;
}


struct nhgui_result
nhgui_font_text_result_centered_by_previous_x(
		struct nhgui_result result,
		struct nhgui_context *context, 
		struct nhgui_font_character character[128],
		const char *text,
		uint32_t text_length
)
{
	/* Compute the length of the text string and center it with regards 
	 * to the previous element size */

	float x_mm = 0.0f;
	for(uint32_t i = 0; i < text_length; i++)
	{
		unsigned char c = text[i];
		float mm_per_pixel_x = (float)context->width_mm/(float)context->res_x;

		if(i != text_length-1)
			x_mm += (character[c].advance_x >> 6) * mm_per_pixel_x;
	}
	result.x_mm += result.x_inc_next/2 - x_mm/2;
	return result;
}



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
)
{
	/* Calculate the maximum delta in y such that the text can be centered in
	 * the rendering loop.
	 **/	

	float delta_y_max = 0;	
	for(uint32_t i = 0; i < text_length; i++)
	{	
		unsigned char c = (unsigned char)text[i];
		float mm_per_pixel_y = (float)context->height_mm/(float)context->res_y;

		float delta  = character[c].height * mm_per_pixel_y;
		
		delta_y_max = delta_y_max < delta ? delta : delta_y_max;
	}



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(instance->program);

	struct nhgui_result render_result = result;

	for(uint32_t i = 0; i < text_length; i++)
	{	
		unsigned char c = (unsigned char)text[i];

		float mm_per_pixel_x = (float)context->width_mm/(float)context->res_x;
		float mm_per_pixel_y = (float)context->height_mm/(float)context->res_y;
		
		float width_mm = character[c].width * mm_per_pixel_x;
		float height_mm = character[c].height * mm_per_pixel_y;
		
		struct nhgui_result result_tmp;	
		result_tmp.x_mm = result.x_mm + character[c].bearing_x * mm_per_pixel_x;
		result_tmp.y_mm = result.y_mm - (character[c].height - character[c].bearing_y) * mm_per_pixel_y - delta_y_max; 
		nhgui_common_uniform_locations_set(&instance->locations, context, input, result_tmp, width_mm, height_mm);
		result.x_mm += (character[c].advance_x >> 6) * mm_per_pixel_x;
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, character[c].texture);

		nhgui_surface_render(&context->surface);
	}

	render_result.x_inc_next = result.x_mm;
	render_result.y_inc_next = delta_y_max;

	glDisable(GL_BLEND);

	return render_result;
}



