#ifndef NHGUI_H
#define NHGUI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdint.h>

#include "misc/file.h"

#include "math/vec.h"

#define NHGUI_INPUT_MAX 32
#define NGGUI_SHADER_FILE_MAX_SIZE 8192

struct nhgui_render_attribute
{
	/* Height of the element */
	float height_mm;	

	/* Width of the elemenet. Usually is it not set 
	 * and determined by the height or input length
	 * of etc. a string */
	float width_mm;
	
	/* Colors of the  object */
	float r,g,b;
};

struct nhgui_result
{
	/* Offset the x_mm and y_mm with these values */
	float x_offset_mm;
	float y_offset_mm;

	/* Current y and x values to draw to */
	float y_mm;
	float x_mm;
	
	/* Next y and x values depending on operation */
	float  y_inc_next;
	float  x_inc_next;
	
	/* Maximum and min of of +x/y + inc */
	float y_min_mm;
	float x_max_mm;
	
};


struct nhgui_input
{
	/* Width and height of the window */
	uint32_t width;
	uint32_t height;

	/* Cursor location with 0,0 in lower left */
	float cursor_x;
	float cursor_y;

	float cursor_x_delta;
	float cursor_y_delta;

	/* > 0 if the left cursor button have been clicked */
	uint32_t cursor_button_left;

	/* > 0 if the left cursor button have been pressed */
	uint32_t cursor_button_left_press;

	/* > 0 if the backspace key have been pressed */
	uint32_t key_backspace_state;
	
	/* Frame delta */
	float deltatime;

	/* Secounds since application start */
	float time;
	
	/* > 0 then deselect selected. */
	uint32_t selected_new;

	/* >  0 then next iteration will selected_new be 1 */
	uint32_t selected_new_raise;
	
	/* Input from keyboard */	
	char input[NHGUI_INPUT_MAX];
	uint32_t input_length;


};

struct nhgui_common_uniform_locations
{
	GLint position;
	GLint size;
	GLint color;
	GLint dimension;
};

struct nhgui_surface
{
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
	GLuint draw_count;
};


struct nhgui_icon_menu_instance
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};

struct nhgui_icon_menu
{
	uint8_t clicked;
};


struct nhgui_icon_text_cursor_instance 
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};

struct nhgui_icon_blank_instance 
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};



struct nhgui_object_font_freetype
{
	FT_Library ft;
};

struct nhgui_object_font_character
{
	/* Opengl texture object */
	GLuint texture;
	
	/* Desired height in mm */
	float height_mm;	

	/* Width and height of character in pixels */
	uint32_t width;
	uint32_t height;

	/* See freetype glyph documentation */
	uint32_t bearing_x;
	uint32_t bearing_y;
	uint32_t advance_x;	
};

struct nhgui_object_font
{
	float height_mm;
	float delta_y_max;
	struct nhgui_object_font_character character[128];
};


struct nhgui_object_font_text_instance 
{
	GLuint program;

	struct nhgui_common_uniform_locations locations;
};

struct nhgui_object_radio_button_instance 
{
	GLuint shader_program;

	GLint location_dimension;
	GLint location_checked;
	GLint location_position;
	GLint location_size;
};

struct nhgui_object_radio_button
{
	uint8_t checked;
};

struct nhgui_icon_blank
{
	uint8_t selected;
	uint8_t selected_prev;

	uint8_t clicked;	

	uint8_t pressed;
};

struct nhgui_object_input_field
{
	/* Cursor position */
	uint32_t cursor_index;

	/* background color of the field */
	struct vec3 field_color;
	
	/* Used for input */	
	struct nhgui_icon_blank blank_object;	
};


struct nhgui_object_text_list
{
	
	/* Color of text and background of none selected */
	struct vec3 text_color;
	struct vec3 field_color;

	/* Color of text and background of selected */
	struct vec3 selected_field_color;
	struct vec3 selected_text_color;
	
	/* > 0, then scroll the text if it overflows */
	float char_scroll_per_sec;
	
	/* > 0 if anything is selected */
	uint32_t selected;

	/* Index of the selcted */
	uint32_t selected_index;
	
	/* Used internaly */	
	uint32_t selected_prev;

	/* Result of the selected background */
	struct nhgui_result selected_result;

};


struct nhgui_context
{
	struct nhgui_surface surface;

	struct nhgui_icon_text_cursor_instance text_cursor;

	struct nhgui_icon_blank_instance blank;

	struct nhgui_object_font_text_instance font;

	struct nhgui_object_radio_button_instance radio_button;

	struct nhgui_icon_menu_instance menu;

	/* Screen width and height in mm. */
	uint32_t width_mm;
	uint32_t height_mm;
	
	/* Screen resolution */
	uint32_t res_x;
	uint32_t res_y;
};

struct nhgui_object_scroll_bar 
{


	/* Internals */

	/* Amount of scroll in mm */
	float scroll_y_mm;
	float scroll_x_mm;	

	/* Used for the scroll bars */
	struct nhgui_icon_blank blank_scroll_y;
	struct nhgui_icon_blank blank_scroll_x;
};

struct nhgui_window 
{
	/* Attributes of the scroll bar. Shall be set. */
	struct nhgui_render_attribute scroll_bar_attribute;

	/* Internals */
	struct nhgui_result result_begin;
	struct nhgui_object_scroll_bar scroll_bar;
};

struct nhgui_object_font_text_area 
{
	struct vec3 background_color;	

	struct vec3 font_color;
};





int nhgui_context_initialize(
		struct nhgui_context *context,
	       	uint32_t res_x, uint32_t res_y,
	       	uint32_t width_mm, uint32_t height_mm
);

void nhgui_context_deinitialize(
		struct nhgui_context *context
);




struct nhgui_result 
nhgui_result_margin(struct nhgui_result result, float margin_x_mm, float margin_y_mm);

struct nhgui_result
nhgui_result_inc_x(struct nhgui_result result);


struct nhgui_result
nhgui_result_dec_y(struct nhgui_result result);

struct nhgui_result
nhgui_result_rewind_x(struct nhgui_result result);

struct nhgui_result
nhgui_result_rewind_x_to(struct nhgui_result result, struct nhgui_result to);

struct nhgui_result
nhgui_result_rewind_y(struct nhgui_result result);

struct nhgui_result
nhgui_object_scroll_bar_scroll_result(
		const struct nhgui_object_scroll_bar *bar,
		const struct nhgui_result result
);

void
nhgui_object_scroll_bar(
		struct nhgui_object_scroll_bar *bar,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *scroll_attribute,
		const struct nhgui_render_attribute *size_attribute,
		struct nhgui_input *input, 
		const struct nhgui_result scroll_result,	
		const struct nhgui_result result	
);

struct nhgui_result
nhgui_window_begin(
		struct nhgui_window *window,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input,
		const struct nhgui_result result
);

struct nhgui_result 
nhgui_window_end(
		struct nhgui_window *window,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input,
		const struct nhgui_result result
);

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

);

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
);

struct nhgui_result 
nhgui_icon_blank_no_object(
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
);

struct nhgui_result 
nhgui_icon_blank(
		struct nhgui_icon_blank *blank,
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		const struct nhgui_result result
);

struct nhgui_result 
nhgui_icon_text_cursor(
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
);


struct nhgui_result
nhgui_icon_menu(
		struct nhgui_icon_menu *object,
		const struct nhgui_context *context, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
);


int 
nhgui_object_font_freetype_initialize(struct nhgui_object_font_freetype *freetype);

void 
nhgui_object_font_freetype_deinitialize(struct nhgui_object_font_freetype *freetype);

int 
nhgui_object_font_freetype_characters_initialize(
		struct nhgui_object_font_freetype *freetype,
		const struct nhgui_context *context,
		const struct nhgui_render_attribute *attribute,
	       	struct nhgui_object_font *font, 
	       	const char *filename
);

void 
nhgui_object_font_freetype_characters_deinitialize(
	       	struct nhgui_object_font *font
);
struct nhgui_result
nhgui_object_font_text_result_centered_by_previous_x(
		const struct nhgui_result result,
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text,
		const uint32_t text_length
);


uint32_t 
nhgui_object_font_text_overflow_count(
		const struct nhgui_result within, 
		const struct nhgui_context *context,
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text,
		const uint32_t text_length
);

float 
nhgui_object_font_text_delta_y_max(
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const struct nhgui_render_attribute *attribute,
		const char *text, 
		const uint32_t text_length 
);

struct nhgui_result
nhgui_object_font_text(
		const struct nhgui_context *context, 
		const struct nhgui_object_font *font,
		const char *text, 
		const uint32_t text_length, 
		const struct nhgui_render_attribute *attribute,
		const struct nhgui_input *input, 
		const struct nhgui_result result
);

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
);



struct nhgui_result
nhgui_object_radio_button(
	       	struct nhgui_object_radio_button *object,
		const struct nhgui_context *context,
	       	const struct nhgui_render_attribute *attribute,
	       	const struct nhgui_input *input,
	       	const struct nhgui_result result
);

/* Internal functions */

int 
nhgui_icon_blank_initialize(struct nhgui_icon_blank_instance *instance);

void
nhgui_icon_blank_deinitialize(struct nhgui_icon_blank_instance *instance);


int 
nhgui_icon_text_cursor_initialize(struct nhgui_icon_text_cursor_instance *instance);

void
nhgui_icon_text_cursor_deinitialize(struct nhgui_icon_text_cursor_instance *instance);


int 
nhgui_object_font_text_initialize(struct nhgui_object_font_text_instance *instance);

void
nhgui_object_font_text_deinitialize(struct nhgui_object_font_text_instance *instance);


int nhgui_object_radio_button_initialize(
		struct nhgui_object_radio_button_instance *instance
);


void nhgui_object_radio_button_deinitialize(
		struct nhgui_object_radio_button_instance *instance
);


int 
nhgui_icon_menu_initialize(
		struct nhgui_icon_menu_instance *instance
);

void
nhgui_icon_menu_deinitialize(
		struct nhgui_icon_menu_instance *instance
);




GLuint nhgui_shader_vertex_create_from_file(
		const char *vertex_source_filename, 
	       	const char *fragment_source_filename
);

GLuint nhgui_shader_vertex_create(
		const char **vertex_source, 
		int32_t *vertex_source_length, 
		uint32_t vertex_source_count, 
		const char **fragment_source,
	       	int32_t *fragment_source_length,
	       	uint32_t fragment_source_count
);

int nhgui_common_uniform_locations_find(
		struct nhgui_common_uniform_locations *locations, 
		const GLuint program
);

void nhgui_common_uniform_locations_set(
		const struct nhgui_common_uniform_locations *locations,
	       	const struct nhgui_context *context,
	       	const struct nhgui_input *input,
	       	const struct nhgui_result result,
	       	const float width_mm, const float height_mm,
		const float r, const float g, const float b
);
int nhgui_surface_initialize(
		struct nhgui_surface *surface
);

void nhgui_surface_deinitialize(
		struct nhgui_surface *surface
);

void nhgui_surface_render(
		const struct nhgui_surface *nhgui_surface
);

void nhgui_surface_render_instanced(
		const struct nhgui_surface *nhgui_surface,
	       	const uint32_t instance_count
);




#endif 
