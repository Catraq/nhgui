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


#include "misc/file.h"

#include "icon/menu.h"
#include "object/radio_button.h"

struct nhgui_font_freetype
{
	FT_Library ft;
};

struct nhgui_font_character
{
	GLuint texture;
	uint32_t width;
	uint32_t height;
	uint32_t bearing_x;
	uint32_t bearing_y;
	uint32_t advance_x;	
};

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
	
	uint32_t pixels_per_mm = context->res_y/context->height_mm;
	FT_Set_Pixel_Sizes(face, 0, attribute->height_mm * pixels_per_mm);
		
	GLuint texture[128];
	glGenTextures(128, texture);

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

struct nhgui_font_text_instance 
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};


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





int main(int args, char *argv[])
{
	GLFWwindow* window = 0;
	
	/* Initialize openGL */	
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint( GLFW_SAMPLES, 16 );

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		fprintf(stderr,"Failed to init GLFW \n");
		glfwTerminate();
		return -1;
	}
	
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

	int width_mm, height_mm;
	glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &width_mm, &height_mm);

	int res_x, res_y;
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	res_x = mode->width;
	res_y = mode->height;

	glfwMakeContextCurrent(window);
	
	/* Disable vertical sync */
	//glfwSwapInterval(0);

  	
       	/* Enable Version 3.3 */
	glewExperimental = 1;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr,"Failed to init Glew \n");
		glfwTerminate();
		return -1;
	}


	/* Clear any errors */
	while( glGetError() != GL_NONE)
	{
		glGetError();
	}
	
	glfwSwapBuffers(window);
	glfwPollEvents();
	
	
	int result = 0;

	struct nhgui_context context;
	result = nhgui_context_initialize(&context, (uint32_t)res_x, (uint32_t)res_y, (uint32_t)width_mm, (uint32_t)height_mm);
	if(result < 0){
		fprintf(stderr, "nhgui_context_initialize() failed. \n");
		exit(EXIT_FAILURE);
	}


	
	struct nhgui_object_radio_button_instance radio_button_instance;
	result = nhgui_object_radio_button_initialize(&radio_button_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_object_radio_button_initialize() failed. \n");
		exit(EXIT_FAILURE);	
	}



	struct nhgui_icon_menu_instance menu_instance;
	result = nhgui_icon_menu_initialize(&menu_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_icon_menu_initialize() failed. \n");
		exit(EXIT_FAILURE);	
	}


	
	struct nhgui_render_attribute font_render_attribute = {
		.height_mm = 10,
	};

	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
	};



	const char *font_filename = "../data/UbuntuMono-R.ttf";
	struct nhgui_font_character character[128];
	struct nhgui_font_character radio_character[128];

	{
		struct nhgui_font_freetype font_freetype;
		result = nhgui_font_freetype_initialize(&font_freetype);
		if(result < 0){
			fprintf(stderr, "nhgui_font_freetype_initilaize() failed. \n");
			exit(EXIT_FAILURE);
		}
		
		result = nhgui_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&font_render_attribute ,
				character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}
	
		result = nhgui_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&radio_render_attribute,
				radio_character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}


		nhgui_font_freetype_deinitialize(&font_freetype);
	}

	struct nhgui_font_text_instance font_text_instance;
	result = nhgui_font_text_initialize(&font_text_instance);
	if(result < 0){
		fprintf(stderr, "nhgui_font_text_initialize() failed. \n");
		exit(EXIT_FAILURE);
	}
	
	

	uint32_t radio_button_row = 5;
	struct 	nhgui_object_radio_button_object radio_button_object[radio_button_row];



	const char *radio_button_text[] = {
		"I accept that you may have my sould.",
		"I agree that the terms of service is good. ",
		"I hope this gui will be useful", 
		"I accept that I lack imagination",
		"I may not have hope and dreams."
	};




	struct nhgui_render_attribute menu_render_attribute = {
		.height_mm = 10,
	};
	struct nhgui_icon_menu_object menu_object;




	int mouse_button_last = GLFW_RELEASE;	
	while(!glfwWindowShouldClose(window))
	{
		/* Get screen pixel size */
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		/* Get input from mouse and mouse buttons */
		double x_cursor, y_cursor;
		glfwGetCursorPos(window, &x_cursor, &y_cursor);

		int mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		uint32_t mouse_button_state = mouse_button == GLFW_RELEASE ?  mouse_button_last != mouse_button ? 1 : 0 : 0;
		mouse_button_last = mouse_button;
		
		struct nhgui_input input = {
			.width = width,
			.height = height,
			.cursor_x = (uint32_t)x_cursor,
			.cursor_y = height - (uint32_t)y_cursor,
			.cursor_button_left = mouse_button_state
		};	

		struct nhgui_result render_result = {
			.x_mm = 0,
			.y_mm = context.height_mm * (float)input.height/(float)context.res_y
		};
		

		glViewport(0, 0, width,height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);


		render_result = nhgui_icon_menu(&context, &menu_instance, &menu_object, &menu_render_attribute, &input, render_result);
		render_result = nhgui_result_dec_y(render_result);

		const char menu_text[] = "menu";

		render_result = nhgui_font_text(
				&context, 
				&font_text_instance,
				radio_character, 
				menu_text,
				sizeof(menu_text),
				&radio_render_attribute,
				&input, 
				render_result
		);

		render_result = nhgui_result_dec_y(render_result);

		if(menu_object.clicked)
		{
			for(uint32_t j = 0; j < radio_button_row; j++)
			{

				render_result = nhgui_result_margin(render_result, 2, 1);

				uint32_t index = j;
				render_result = nhgui_object_radio_button(
						&context,
					       	&radio_button_instance, 
						&radio_button_object[index], 
						&radio_render_attribute, 
						&input,
					       	render_result
				);
				
				render_result = nhgui_result_inc_x(render_result);
				render_result = nhgui_result_margin(render_result, 2, 0);

				render_result = nhgui_font_text(
						&context, 
						&font_text_instance,
						radio_character, 
						radio_button_text[j], 
						strlen(radio_button_text[j]), 
						&radio_render_attribute,
						&input, 
						render_result
				);

				

				render_result = nhgui_result_rewind_x(render_result);
				render_result = nhgui_result_dec_y(render_result);

			}
					
		}
		
		const char test_string[] = "Test string";

		nhgui_font_text(
				&context, 
				&font_text_instance,
				character, 
				test_string, 
				sizeof(test_string), 
				&font_render_attribute,
				&input, 
				render_result
		);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_object_radio_button_deinitialize(&radio_button_instance);
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
