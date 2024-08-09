#ifndef NHGUI_ERROR_H
#define NHGUI_ERROR_H 

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ ":" S2(__LINE__) ":"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>


void nhgui_gl_error(const char *msg);

#define CHECK_GL_ERROR() nhgui_gl_error(LOCATION)

#endif 
