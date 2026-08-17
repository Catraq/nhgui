#ifndef NHGUI_H
#define NHGUI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdint.h>

#include "math/vec.h"

#include "rl_gui_error.h"


#define RL_GUI_INPUT_MAX 32

/* 
 * Attributes of the rendered object. Note that the attribute 
 * that actually is used depends on the object. It might 
 * be the case that the attribute is determined by the instance 
 * of the object in question.
 **/

struct rl_gui_render_attribute
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

/* 
 * Used for determening where the object shall be drawn. Returned 
 * by each object after drawing to indicate next position to draw to
 */
struct rl_gui_result
{
	/* Offset the x_mm and y_mm with these values */
	float x_offset_mm;
	float y_offset_mm;

	/* Current y and x values to draw to */
	float y_mm;
	float x_mm;
	
	/* Next increment of y and x values, is set by the operation */
	float  y_inc_next;
	float  x_inc_next;
};

/* Input from the user.
 *
 * Also used for managing input of the objects. 
 * */
struct rl_gui_input
{	
	/* Frame delta */
	float deltatime_sec;

	/* Secounds since application start */
	float time_sec;

	/* Width and height of the window in pixels*/
	uint32_t width_pixel;
	uint32_t height_pixel;

	/* Cursor location with 0,0 in lower left */
	float cursor_x_pixel;
	float cursor_y_pixel;
	
	/* Pixel delta from last frame */
	float cursor_x_delta_pixel;
	float cursor_y_delta_pixel;

	/* > 0 if the left cursor button have been clicked */
	uint32_t cursor_button_left;

	/* > 0 if the left cursor button have been pressed */
	uint32_t cursor_button_left_press;

	/* > 0 if the backspace key have been pressed */
	uint32_t key_backspace_state;
	
	/* > 0 then deselect selected. */
	uint32_t selected_new;

	/* >  0 then next iteration will selected_new be 1.
	 *
	 * If a object is selected or pressed is this raised such 
	 * that previous object can look for selected_new > 0 next
	 * iteration and become deselected 
	 * */
	uint32_t selected_new_raise;
	
	/* Input from keyboard */	
	uint8_t input[RL_GUI_INPUT_MAX];
	uint32_t input_length;


};

/* 
 * Opengl uniform locations in the shader
 */
struct rl_gui_common_uniform_locations
{
	GLint position;
	GLint size;
	GLint color;
	GLint dimension;
};

/* Quad used for rendering */
struct rl_gui_surface
{
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
	GLuint draw_count;
};

/* Each object have a instance that is used for seting up the rendering 
 * of the object 
 */


struct rl_gui_icon_menu_instance
{
	GLuint program;

	struct rl_gui_common_uniform_locations locations;
};


struct rl_gui_icon_text_cursor_instance 
{
	GLuint program;

	struct rl_gui_common_uniform_locations locations;
};

struct rl_gui_icon_blank_instance 
{
	GLuint program;

	struct rl_gui_common_uniform_locations locations;
};



struct rl_gui_object_font
{
	/* Used by freetype */
	FT_Face face;

};


struct rl_gui_object_font_text_instance 
{
	GLuint program;

	struct rl_gui_common_uniform_locations locations;
};


struct rl_gui_object_radio_button_instance 
{

	GLuint shader_program;
	
	GLuint location_checked;

	struct rl_gui_common_uniform_locations locations;
};


struct rl_gui_icon_menu
{
	uint8_t clicked;
};

struct rl_gui_object_radio_button
{
	uint8_t checked;
};

struct rl_gui_icon_blank
{
	/* 0 > If the object is selected */
	uint8_t selected;
	
	/* 0 > If the object went from selected to not selected. */
	uint8_t deselected;
	
	/* 0 > If the object was selected on this iteration */
	uint8_t selected_prev;
	
	/* 0 > If the object was clicked */
	uint8_t clicked;	
	
	/* 0 > If the object was pressed */
	uint8_t pressed;
};

struct rl_gui_object_input_field
{
	/* Cursor position */
	uint32_t cursor_index;

	/* background color of the field */
	struct rl_gui_vec3 field_color;
	
	/* Used for input */	
	struct rl_gui_icon_blank blank_object;	
};

struct rl_gui_object_input_field_float 
{
	/* Float number represented as string */
	char str[32];
	
	/* Length of str number */
	uint32_t str_length;
	
	/* > 0 if str have been initialized with the float value passed 
	 * by the function 
	 */
	uint32_t str_initialized;

	struct rl_gui_object_input_field field;
	
};


struct rl_gui_object_text_list
{
	
	/* Color of text and background of none selected */
	struct rl_gui_vec3 text_color;
	struct rl_gui_vec3 field_color;

	/* Color of text and background of selected */
	struct rl_gui_vec3 selected_field_color;
	struct rl_gui_vec3 selected_text_color;
	
	/* > 0, then scroll the text if it overflows */
	float char_scroll_per_sec;
	
	/* > 0 if anything is selected */
	uint32_t selected;

	/* Index of the selcted */
	uint32_t selected_index;
	
	/* Used internaly */	
	uint32_t selected_prev;

	/* Result of the selected background */
	struct rl_gui_result selected_result;

};



struct rl_gui_context
{
	//Freetype instance 
	FT_Library ft;

	/* Surface used for rendering */
	struct rl_gui_surface surface;

	/* Instance of each of the objects. Used for rendering */

	struct rl_gui_icon_text_cursor_instance text_cursor;

	struct rl_gui_icon_blank_instance blank;

	struct rl_gui_object_font_text_instance font;

	struct rl_gui_object_radio_button_instance radio_button;

	struct rl_gui_icon_menu_instance menu;

	/* Screen width and height in mm. */
	uint32_t screen_width_mm;
	uint32_t screen_height_mm;
	
	/* Screen resolution */
	uint32_t screen_resolution_x;
	uint32_t screen_resolution_y;
};

struct rl_gui_object_font_text_area 
{
	struct rl_gui_vec3 background_color;	

	struct rl_gui_vec3 font_color;
};


/* 
 * Initialize the rl_gui context. 
 *
 * It is the first function that should be called.
 */


int rl_gui_context_initialize(
		struct rl_gui_context *context,
	       	uint32_t screen_resolution_x, uint32_t screen_resolution_y,
	       	uint32_t screen_width_mm, uint32_t screen_height_mm
);

void rl_gui_context_deinitialize(
		struct rl_gui_context *context
);

struct rl_gui_result 
rl_gui_result_begin(
		struct rl_gui_input *input,
		struct rl_gui_context *context
		);


/* 
 * Add margin in x and y direction. Dont forget to add margin after drawing the object
 * for margin on both sides
 */
struct rl_gui_result 
rl_gui_result_margin(struct rl_gui_result result, float margin_x_mm, float margin_y_mm);

/* 
 * Move the x-cooordinate past the previous drawed object in x direction.
 * */
struct rl_gui_result
rl_gui_result_inc_x(struct rl_gui_result result);


/* 
 * Move the y-cooordinate past the previous drawed object in y direction.
 * */
struct rl_gui_result
rl_gui_result_dec_y(struct rl_gui_result result);

/* 
 * Rewind the x coordinate to 0 
 */
struct rl_gui_result
rl_gui_result_rewind_x(struct rl_gui_result result);

/* 
 * Rewind the x coordinate to the same x as in to 
 */
struct rl_gui_result
rl_gui_result_rewind_x_to(struct rl_gui_result result, struct rl_gui_result to);


/*
 * attribute height sets font height and width sets the width 
 * of the list object. 
 */
struct rl_gui_result
rl_gui_object_text_list(
		struct rl_gui_object_text_list *list,
		const struct rl_gui_context *context,
		const char *entry[],
		const uint32_t *entry_length,
		const uint32_t entry_count,
		const struct rl_gui_object_font *font,
		const struct rl_gui_render_attribute *attribute,
		struct rl_gui_input *input, 
		const struct rl_gui_result result

);


/*
 * attribute sets the width of the  input field and height 
 * sets the font height. 
 */
struct rl_gui_result 
rl_gui_object_input_field(
		struct rl_gui_object_input_field *field,
		const struct rl_gui_context *context,
		const struct rl_gui_object_font *font,
		const struct rl_gui_render_attribute *attribute,
		struct rl_gui_input *input, 
		const struct rl_gui_result result,
		char *input_buffer, 
		uint32_t *input_buffer_length,
		const uint32_t input_buffer_size
);

/*
 * attribute sets the width of the  input field and height 
 * sets the font height. 
 */
struct rl_gui_result 
rl_gui_object_input_field_float(
		struct rl_gui_object_input_field_float *float_field,
		const struct rl_gui_context *context,
		const struct rl_gui_object_font *font,
		const struct rl_gui_render_attribute *attribute,
		struct rl_gui_input *input, 
		const struct rl_gui_result result,
		float *value
);


/* 
 * Draw quad with as described by attribute 
 */
struct rl_gui_result 
rl_gui_icon_blank_no_object(
		const struct rl_gui_context *context, 
		const struct rl_gui_render_attribute *attribute,
		const struct rl_gui_input *input, 
		const struct rl_gui_result result
);

/* 
 * Draw quad but support input 
 */
struct rl_gui_result 
rl_gui_icon_blank(
		struct rl_gui_icon_blank *blank,
		const struct rl_gui_context *context, 
		const struct rl_gui_render_attribute *attribute,
		struct rl_gui_input *input, 
		const struct rl_gui_result result
);

/* 
 * Blinking text cursor. Attribute height sets both
 * width and height 
 */
struct rl_gui_result 
rl_gui_icon_text_cursor(
		const struct rl_gui_context *context, 
		const struct rl_gui_render_attribute *attribute,
		const struct rl_gui_input *input, 
		const struct rl_gui_result result
);


/* 
 * Draw a menu icon. Attribute height sets both width and 
 * height.
 */
struct rl_gui_result
rl_gui_icon_menu(
		struct rl_gui_icon_menu *object,
		const struct rl_gui_context *context, 
		const struct rl_gui_render_attribute *attribute,
		const struct rl_gui_input *input, 
		const struct rl_gui_result result
);


/* 
 * Load ANSI characters from filename into font with height described in attribute.
 */
int 
rl_gui_object_font_freetype_characters_initialize(
		const struct rl_gui_context *context,
		const struct rl_gui_render_attribute *attribute,
	       	struct rl_gui_object_font *font, 
	       	const char *filename
);

void 
rl_gui_object_font_freetype_characters_deinitialize(
	       	struct rl_gui_object_font *font
);

/* 
 * Used for determening result such that text can be centered 
 * around x of previous resut where x inc have not been called. 
 *
 * Attribute should be the same as the font is rendered with.
 * */
struct rl_gui_result
rl_gui_object_font_text_result_centered_by_previous_x(
		const struct rl_gui_result result,
		const struct rl_gui_context *context, 
		const struct rl_gui_object_font *font,
		const struct rl_gui_render_attribute *attribute,
		const char *text,
		const uint32_t text_length
);

/* 
 * Calucalte number of characters of the text overflows 
 * where within.x_mm is the starting point and 
 * within.x_inc_next is the ending point. 
 */
uint32_t 
rl_gui_object_font_text_overflow_count(
		const struct rl_gui_result within, 
		const struct rl_gui_context *context,
		const struct rl_gui_object_font *font,
		const char *text,
		const uint32_t text_length
);

/* 
 * Draw text with height described in attribute 
 */
struct rl_gui_result
rl_gui_object_font_text(
		const struct rl_gui_context *context, 
		const struct rl_gui_object_font *font,
		const char *text, 
		const uint32_t text_length, 
		const struct rl_gui_render_attribute *attribute,
		const struct rl_gui_input *input, 
		const struct rl_gui_result result
);

/* 
 * Draw text area with width described in attribute.
 */

struct rl_gui_result 
rl_gui_object_font_text_area(
		const struct rl_gui_object_font_text_area *area,
		const struct rl_gui_context *context,
		const struct rl_gui_object_font *font,
		const struct rl_gui_render_attribute *attribute,
		const struct rl_gui_input *input, 
		const struct rl_gui_result result,
		const char *input_buffer, 
		const uint32_t input_buffer_size
);


/* 
 * Attribute height describes both height and width of the radio button. 
 */

struct rl_gui_result
rl_gui_object_radio_button(
	       	struct rl_gui_object_radio_button *object,
		const struct rl_gui_context *context,
	       	const struct rl_gui_render_attribute *attribute,
	       	const struct rl_gui_input *input,
	       	const struct rl_gui_result result
);



#endif 
