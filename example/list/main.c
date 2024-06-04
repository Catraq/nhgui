#include <stdio.h>
#include <stdint.h>

#include <string.h>

#include "math/vec.h"
#include "nhgui_glfw.h"
#include "nhgui.h"



struct nhgui_object_text_list
{
	uint32_t width_mm;

	struct vec3 text_color;
	struct vec3 field_color;

	struct vec3 selected_field_color;
	struct vec3 selected_text_color;

	uint32_t selected;
	uint32_t selected_index;

	struct nhgui_result selected_result;

	uint32_t selected_prev;
};

struct nhgui_result
nhgui_object_text_list(
		struct nhgui_context *context,
		struct nhgui_object_text_list *list,
		char *entry[],
		uint32_t *entry_length,
		uint32_t entry_count,
		struct nhgui_object_font_character character[128], 
		struct nhgui_render_attribute *attribute,
		struct nhgui_input *input, 
		struct nhgui_result result

)
{

	float cursor_x_mm = (float)input->width / (float)context->res_x * (float)context->width_mm/(float)input->width * (float)input->cursor_x;
	float cursor_y_mm = (float)input->height / (float)context->res_y * (float)context->height_mm/(float)input->height * (float)input->cursor_y;

	if(list->selected_prev > 0)
	{
		list->selected_prev = 0;
		input->selected_new = 0;	
	}
	else if(input->selected_new > 0)
	{
		list->selected = 0;	
	}	
	
	for(uint32_t i = 0; i < entry_count; i++)
	{
		

		struct nhgui_result result_tmp = result;
		result_tmp.y_mm -= attribute->height_mm;
		
		if(input->cursor_button_left > 0)
		{	
			if(cursor_x_mm > result_tmp.x_mm && cursor_x_mm < result_tmp.x_mm + list->width_mm 
			&& cursor_y_mm > result_tmp.y_mm && cursor_y_mm < result_tmp.y_mm + attribute->height_mm)
			{
				if(list->selected_index == i)
					list->selected = list->selected ? 0 : 1;
				else
					list->selected = 1;

				list->selected_prev = list->selected;

				list->selected_index = i;

				if(list->selected > 0)
				{
					input->selected_new = 1;	
				}	
			}	
		}

		if(list->selected > 0 && list->selected_index == i)
		{
		
			struct nhgui_render_attribute selected_attribute = {
				.height_mm = attribute->height_mm,
				.width_mm = list->width_mm,
				.r = list->selected_field_color.x,
				.g = list->selected_field_color.y,
				.b = list->selected_field_color.z,
			};

			result = nhgui_icon_blank_no_object(
					context,
					&selected_attribute,
					input,
					result
			);

			list->selected_result = result;
			
			struct nhgui_render_attribute selected_font_attribute = *attribute;
			selected_font_attribute.r = list->selected_text_color.x;
			selected_font_attribute.g = list->selected_text_color.y;
			selected_font_attribute.b = list->selected_text_color.z;

			nhgui_object_font_text(
					context, 
					character, 
					entry[i],
					entry_length[i],
					&selected_font_attribute,
					input, 
					result
			);
		}
		else
		{

			struct nhgui_render_attribute _attribute = {
				.height_mm = attribute->height_mm,	
				.width_mm = list->width_mm,
				.r = list->field_color.x,
				.g = list->field_color.y,
				.b = list->field_color.z,
			};


			result = nhgui_icon_blank_no_object(
					context,
					&_attribute,
					input,
					result
			);

	
			struct nhgui_render_attribute font_attribute = *attribute;
			font_attribute.r = list->text_color.x;
			font_attribute.g = list->text_color.y;
			font_attribute.b = list->text_color.z;

			nhgui_object_font_text(
					context, 
					character, 
					entry[i],
					entry_length[i],
					&font_attribute,
					input, 
					result
			);
		}

		if(i != entry_count - 1)
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


	glfwSetCharCallback(window, nhgui_glfw_char_callback);

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

	





	/* Add input field */
	const uint32_t add_buffer_size = 32;
	uint32_t add_buffer_length = 0;
	char add_buffer[add_buffer_size];
	
	/* List with elements */	
	uint32_t add_text_count = 10;	
	uint32_t add_text_used = 0;
	char *add_text_buffer_ptr[add_text_count];
	memset(add_text_buffer_ptr, 0, sizeof add_text_buffer_ptr);

	uint8_t add_text_buffer[add_text_count][add_buffer_size];
	uint32_t add_text_buffer_length[add_text_count];
	memset(add_text_buffer_length, 0, sizeof(add_text_buffer_length));


	struct nhgui_object_input_field add_field = {
		.width_mm = 70,
	};

	struct nhgui_object_text_list list_object  = 
	{
		.width_mm = 70,
		.text_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
		.field_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_text_color = {.x = 0.0, .y = 0.0, .z = 0.0},	
		.selected_field_color = {.x = 1.0, .y = 1.0, .z = 1.0},	
	
	};
	
	struct nhgui_icon_blank_object add_blank;
	struct nhgui_icon_blank_object delete_blank;

	struct nhgui_result render_result = {};
	struct nhgui_glfw_frame frame = nhgui_frame_create();

	while(!glfwWindowShouldClose(window))
	{

		struct nhgui_input input = nhgui_glfw_frame_begin(&frame, window);

		glViewport(0, 0, input.width, input.height);
		glClearColor(0.1, 0.5, 0.5, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		struct nhgui_result render_result_start = render_result;
		render_result_start.x_mm = 0;
		render_result_start.y_mm = context.height_mm * (float)input.height/(float)context.res_y;
		
		
		struct nhgui_result input_res = nhgui_result_margin(render_result_start, 1, 1);
		
		struct nhgui_result res = nhgui_object_input_field(
				&context,
				&add_field, 
				character, 
				&radio_render_attribute,
				&input, 
				input_res,
				add_buffer, 
				&add_buffer_length,
				add_buffer_size
		);
		res = nhgui_result_inc_x(res);	

		struct nhgui_render_attribute blank_attribute = {
			.height_mm = res.y_inc_next,
			.width_mm = 10,	
			.r = 1.0,
		};
		
		const char add_text[] = "Add";

		res = nhgui_icon_blank(
			&context,
			&add_blank,
			&blank_attribute,
			&input,
			res
		);

		struct nhgui_result icon_blank_center = nhgui_object_font_text_result_centered_by_previous_x(
				res, 
				&context,
				character,
				&radio_render_attribute,
				add_text,
				sizeof(add_text)
		);


		nhgui_object_font_text(
				&context,
				character,
				add_text,
				sizeof(add_text),
				&radio_render_attribute,
				&input, 
				icon_blank_center
		);

	

		if(add_blank.clicked > 0)
		{
			add_blank.clicked = 0;
			if(add_text_used < add_text_count)
			{
				add_text_used++;

				uint32_t free_index = 0;
				for(uint32_t i = 0 ; i < add_text_count; i++){
					if(add_text_buffer_ptr[i] == NULL){
						free_index = i;
						break;
					}
				}


				memcpy(add_text_buffer[free_index], add_buffer, add_buffer_size);	
				add_text_buffer_length[free_index] = add_buffer_length;
				add_text_buffer_ptr[free_index] = &add_text_buffer[free_index];

				add_buffer_length = 0;
			}

		
		}






		res = nhgui_result_dec_y(res);	
		res = nhgui_result_rewind_x_to(res, input_res);

		res = nhgui_object_text_list(
				&context, 
				&list_object,
				add_text_buffer_ptr,
				add_text_buffer_length,
				add_text_count, 
				character, 
				&radio_render_attribute,
				&input, 
				res
		);
		

		if(add_text_used > 0)
		{	if(list_object.selected > 0)
			{	
				const char delete_text[] = "Delete";
				
				res = nhgui_result_inc_x(list_object.selected_result);

				res = nhgui_icon_blank(
					&context,
					&delete_blank,
					&blank_attribute,
					&input,
					res	
				);

				struct nhgui_result icon_blank_delete_center = nhgui_object_font_text_result_centered_by_previous_x(
						res, 
						&context,
						character,
						&radio_render_attribute,
						delete_text,
						sizeof(delete_text)
				);


				nhgui_object_font_text(
						&context,
						character,
						delete_text,
						sizeof(delete_text),
						&radio_render_attribute,
						&input, 
						icon_blank_delete_center
				);

				if(delete_blank.clicked > 0)
				{
					delete_blank.clicked = 0;

					uint32_t index = list_object.selected_index;
					add_text_buffer_ptr[index] = NULL;
					add_text_buffer_length[index] = 0;

					if(add_text_used != 0)
						add_text_used--;

					list_object.selected = 0;

				}
			}
		}
	
		nhgui_glfw_frame_end(&frame, &input);	
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	nhgui_context_deinitialize(&context);

	glfwDestroyWindow(window);
    	glfwTerminate();
}
