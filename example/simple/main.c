#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include <sys/time.h>

#include "nhgui.h"

#define GLFW_CHARACTER_CALLBACK_BUFFER_SIZE 32 



uint32_t character_callback_buffer_index;
char character_callback_buffer[GLFW_CHARACTER_CALLBACK_BUFFER_SIZE];

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
	//ANSI CHARACTERS 
	if(codepoint > 30 && codepoint < 127){
		if(character_callback_buffer_index < GLFW_CHARACTER_CALLBACK_BUFFER_SIZE)
		{
			character_callback_buffer[character_callback_buffer_index] = codepoint;
			character_callback_buffer_index++;
		}
	}
}

void character_callback_erase_char()
{
	if(character_callback_buffer_index > 0)
		character_callback_buffer_index--;
}
void
character_callback_erase_buffer()
{
	character_callback_buffer_index = 0;
}

uint32_t
nhgui_input_text(char *buffer, uint32_t buffer_length, uint32_t buffer_size)
{
	for(uint32_t i = 0; i < character_callback_buffer_index; i++)
	{
		if(buffer_length + i < buffer_size)
			buffer[buffer_length + i] = character_callback_buffer[i];
		else
		{
			character_callback_buffer_index -= i;
			return buffer_length + i;
		}

	}
	
	uint32_t ret =	buffer_length + character_callback_buffer_index;
	character_callback_buffer_index = 0;
	return ret;
}


struct nhgui_result 
nhgui_object_input_field(
		struct nhgui_context *context,
		struct nhgui_object_input_field *field,
		struct nhgui_object_font_character character[128], 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result,
		char *input_buffer, 
		uint32_t *input_buffer_length,
		uint32_t input_buffer_size
)
{

	
	float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
	float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;
	
	struct nhgui_result result_tmp = result;
	result_tmp.y_mm -= attribute->height_mm;
	
	if(field->selected_prev > 0)
	{
		field->selected_prev = 0;
		input->selected_new = 0;
	}
	else if(input->selected_new > 0)
	{
		field->selected = 0;	
	}	
	else if(input->cursor_button_left > 0)
	{
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			field->selected = field->selected ? 0 : 1;
			field->selected_prev = field->selected;

			if(field->selected > 0){
				input->selected_new = 1;	
			}
		}	
	
	}
	else if(field->selected > 0)
	{
		/* Search input */
		*input_buffer_length = nhgui_input_text(input_buffer, *input_buffer_length, input_buffer_size);
		if(input->key_backspace_state  > 0){
			if(*input_buffer_length > 0)
			{
				*input_buffer_length -= 1;	
			}
		} 
	}


	/* Background of the input field */
	struct nhgui_result ret = nhgui_icon_blank(
			context,
			attribute,
			input,
			result
	);



	/* Find result that is past the last character and 
	 * determine if any characets leaks past the input 
	 * box. Note that the cursor is a character*/
	struct nhgui_result res = result;
	uint32_t count = *input_buffer_length < input_buffer_size ? *input_buffer_length : input_buffer_size;

	float x_mm = 0.0f;
	uint32_t overflow_count = 0;
	for(uint32_t i = 0; i < count; i++)
	{
		unsigned char c = input_buffer[i];
		float mm_per_pixel_x = (float)context->width_mm/(float)context->res_x;
		
		float cursor_mm = attribute->height_mm;
		float new_x_mm = x_mm + (float)(character[c].advance_x >> 6) * mm_per_pixel_x + cursor_mm;
		/* See if it is past the boudning box */
		if(res.x_mm + new_x_mm > ret.x_mm + ret.x_inc_next)
		{
			overflow_count++;		
		}
		else{
			x_mm += (character[c].advance_x >> 6) * mm_per_pixel_x;
		}
	}
	res.x_mm += x_mm;
	
	struct nhgui_render_attribute font_attribute = 
	{
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};

	/* This is cursor placed behind the text */
	if(field->selected > 0)
	{
		nhgui_icon_text_cursor(
				context,
				&font_attribute,
				input,
				res
		);
	}

	nhgui_object_font_text(
			context, 
			character, 
			&input_buffer[overflow_count],
			*input_buffer_length - overflow_count,
			&font_attribute,
			input, 
			result
	);
	
	result.y_inc_next = attribute->height_mm;
	result.x_inc_next = x_mm;
	return result;

}


struct nhgui_frame 
{
	uint32_t backspace_key_last;
	uint32_t mouse_button_last;
	uint32_t input_selected_new;

	struct timeval time_curr_time;
	float total_time;
};

struct nhgui_frame
nhgui_frame_create()
{
	struct nhgui_frame frame;

	frame.backspace_key_last = GLFW_RELEASE;
	frame.mouse_button_last = GLFW_RELEASE;	
	
	frame.total_time = 0.0f;

	gettimeofday(&frame.time_curr_time, NULL);
	
	frame.input_selected_new = 0;	
	
	return frame;	
}

void 
nhgui_glfw_frame_end(struct nhgui_frame *frame, struct nhgui_input *input)
{
	frame->input_selected_new = input->selected_new;
}

struct nhgui_input 
nhgui_glfw_frame_begin(struct nhgui_frame *frame, GLFWwindow *window)
{
	struct timeval time_curr_tmp;
	gettimeofday(&time_curr_tmp, NULL);
	float deltatime = time_curr_tmp.tv_sec - frame->time_curr_time.tv_sec + (float)(time_curr_tmp.tv_usec - frame->time_curr_time.tv_usec)/(1000.0f*1000.0f);
	frame->time_curr_time = time_curr_tmp;
	frame->total_time += deltatime;



	int backspace_key = glfwGetKey(window, GLFW_KEY_BACKSPACE);
	uint32_t backspace_key_state  = backspace_key == GLFW_RELEASE ?  frame->backspace_key_last != backspace_key ? 1 : 0 : 0;
	frame->backspace_key_last = backspace_key;
	if(frame->backspace_key_last)
	{
		character_callback_erase_char();
	}

	/* Get screen pixel size */
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	/* Get input from mouse and mouse buttons */
	double x_cursor, y_cursor;
	glfwGetCursorPos(window, &x_cursor, &y_cursor);

	int mouse_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	uint32_t mouse_button_state = mouse_button == GLFW_RELEASE ?  frame->mouse_button_last != mouse_button ? 1 : 0 : 0;
	frame->mouse_button_last = mouse_button;
	
	struct nhgui_input input = {
		.width = width,
		.height = height,
		.cursor_x = (uint32_t)x_cursor,
		.cursor_y = height - (uint32_t)y_cursor,
		.cursor_button_left = mouse_button_state,
		.key_backspace_state = backspace_key_state,
		.deltatime = deltatime,
		.time = frame->total_time,
		.selected_new = frame->input_selected_new,

	};	
	if(frame->input_selected_new > 0){
		character_callback_erase_buffer();	
	}
	frame->input_selected_new = 0;

	return input;		
}

struct nhgui_object_text_list_object
{
	uint32_t selected;
	uint32_t selecteed_index;
};


struct nhgui_result
nhgui_object_text_list(
		struct nhgui_context *context,
		struct nhgui_object_text_list_object *object,
		char *entry[],
		uint32_t *entry_length,
		uint32_t entry_count,
		struct nhgui_object_font_character character[128], 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result

)
{
	
	struct nhgui_render_attribute font_attribute = 
	{
		.height_mm = attribute->height_mm,
		.r = 1.0f,	
		.g = 1.0f, 
		.b = 1.0f
	};

	for(uint32_t i = 0; i < entry_count; i++)
	{
#if 0
		float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
		float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;
	
		struct nhgui_result result_tmp = result;
		result_tmp.y_mm -= attribute->height_mm;
		
		if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + attribute->width_mm 
		&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
		{
			field->selected = field->selected ? 0 : 1;
			field->selected_prev = field->selected;

			if(field->selected > 0){
				input->selected_new = 1;	
			}
		}	
#endif 

		nhgui_icon_blank(
				context,
				attribute,
				input,
				result
		);


		result = nhgui_object_font_text(
				context, 
				character, 
				entry[i],
				entry_length[i],
				&font_attribute,
				input, 
				result
		);

		result = nhgui_result_dec_y(result);

	}

	return result;
}



int main(int args, char *argv[])
{

	/* First initialize opengl context */
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


	character_callback_buffer_index = 0;
	glfwSetCharCallback(window, character_callback);

	glfwMakeContextCurrent(window);
	
	/* Disable vertical sync */
	//glfwSwapInterval(0);

  	
       	/* Enable Version 3.3 */
	glewExperimental = 1;
	if(glewInit() != GLEW_OK)
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
	/* Create gui context */	
	struct nhgui_context context;
	result = nhgui_context_initialize(&context, (uint32_t)res_x, (uint32_t)res_y, (uint32_t)width_mm, (uint32_t)height_mm);
	if(result < 0){
		fprintf(stderr, "nhgui_context_initialize() failed. \n");
		exit(EXIT_FAILURE);
	}


	
	struct nhgui_render_attribute font_render_attribute = {
		.height_mm = 10,
	};

	struct nhgui_render_attribute radio_render_attribute = {
		.height_mm = 3,
		.width_mm = 40, 
	};



	const char *font_filename = "../data/UbuntuMono-R.ttf";
	struct nhgui_object_font_character character[128];
	struct nhgui_object_font_character radio_character[128];

	{
		struct nhgui_object_font_freetype font_freetype;
		result = nhgui_object_font_freetype_initialize(&font_freetype);
		if(result < 0){
			fprintf(stderr, "nhgui_object_font_freetype_initilaize() failed. \n");
			exit(EXIT_FAILURE);
		}
		
		result = nhgui_object_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&font_render_attribute ,
				character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_object_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}
	
		result = nhgui_object_font_freetype_characters_initialize(
				&font_freetype,
				&context,
				&radio_render_attribute,
				radio_character, 
				font_filename
		);

		if(result < 0)
		{
			fprintf(stderr, "nhgui_object_font_freetype_characters_initialize() failed. \n");
			exit(EXIT_FAILURE);
		}


		nhgui_object_font_freetype_deinitialize(&font_freetype);
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

	/* Search input field */
	const uint32_t input_buffer_size = 32;
	uint32_t input_buffer_length = 0;
	char input_buffer[input_buffer_size];
	struct nhgui_object_input_field input_field;

	/* Add input field */
	const uint32_t add_buffer_size = 32;
	uint32_t add_buffer_length = 0;
	char add_buffer[input_buffer_size];
	struct nhgui_object_input_field add_field;

	struct nhgui_object_text_list_object list_object;
	uint32_t list_entries_length[] = 
	{
		strlen(radio_button_text[0]), 	
		strlen(radio_button_text[1]), 	
		strlen(radio_button_text[2]), 	
		strlen(radio_button_text[3]), 	
		strlen(radio_button_text[4]), 	
	};

	struct nhgui_result render_result = {};
	struct nhgui_frame frame = nhgui_frame_create();

	while(!glfwWindowShouldClose(window))
	{

		struct nhgui_input input = nhgui_glfw_frame_begin(&frame, window);

		glViewport(0, 0, input.width, input.height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		struct nhgui_result render_result_start = render_result;
		render_result_start.x_mm = 0;
		render_result_start.y_mm = context.height_mm * (float)input.height/(float)context.res_y;
		
		
		/* Menu button */
		render_result = nhgui_result_margin(render_result_start, 1, 1);
		struct nhgui_result m_render_result = nhgui_icon_menu(&context, &menu_object, &menu_render_attribute, &input, render_result);

		render_result = nhgui_result_dec_y(m_render_result);
		m_render_result = nhgui_result_inc_x(m_render_result);

		/* Menu button text */
		const char menu_text[] = "menu";
		render_result = nhgui_result_margin(render_result, 0, 1);
		struct nhgui_result c_render_result = nhgui_object_font_text_result_centered_by_previous_x(
				render_result, 
				&context, 
				radio_character, 
				menu_text,
				sizeof(menu_text)
		);


		render_result = nhgui_object_font_text(
				&context, 
				radio_character, 
				menu_text,
				sizeof(menu_text),
				&radio_render_attribute,
				&input, 
				c_render_result
		);

		render_result = nhgui_result_dec_y(render_result);

		struct nhgui_result res = nhgui_object_input_field(
				&context,
				&input_field, 
				radio_character, 
				&radio_render_attribute,
				&input, 
				m_render_result,
				input_buffer, 
				&input_buffer_length,
				input_buffer_size
		);
		
		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, m_render_result); 
		res = nhgui_result_margin(res, 0, 1);	
			
		res = nhgui_object_input_field(
				&context,
				&add_field, 
				radio_character, 
				&radio_render_attribute,
				&input, 
				res,
				add_buffer, 
				&add_buffer_length,
				add_buffer_size
		);

		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, m_render_result); 

		res = nhgui_object_text_list(
				&context, 
				&list_object,
				radio_button_text,
				list_entries_length,
				5, 
				radio_character, 
				&radio_render_attribute,
				&input, 
				res
		);


		
	



		if(menu_object.clicked)
		{
			struct nhgui_result radio_init_result = c_render_result;
			for(uint32_t j = 0; j < radio_button_row; j++)
			{
				render_result.x_mm = radio_init_result.x_mm;

				render_result = nhgui_result_margin(render_result, 0, 1);

				uint32_t index = j;
				render_result = nhgui_object_radio_button(
						&context,
						&radio_button_object[index], 
						&radio_render_attribute, 
						&input,
					       	render_result
				);
				
				render_result = nhgui_result_inc_x(render_result);
				render_result = nhgui_result_margin(render_result, 2, 0);

				render_result = nhgui_object_font_text(
						&context, 
						radio_character, 
						radio_button_text[j], 
						strlen(radio_button_text[j]), 
						&radio_render_attribute,
						&input, 
						render_result
				);

				
				render_result = nhgui_result_margin(render_result, 0, 1);

				render_result = nhgui_result_rewind_x(render_result);
				render_result = nhgui_result_dec_y(render_result);

			}
					
		}
		
		const char test_string[] = "Test string";

		nhgui_object_font_text(
				&context, 
				character, 
				test_string, 
				sizeof(test_string), 
				&font_render_attribute,
				&input, 
				render_result
		);

		nhgui_glfw_frame_end(&frame, &input);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
