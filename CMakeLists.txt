cmake_minimum_required(VERSION 3.0.2)
project(nhgui)


find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIR})

find_package(glfw3 3.0 REQUIRED)

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})



include_directories("${PROJECT_SOURCE_DIR}/src/")
include_directories("${PROJECT_SOURCE_DIR}/include/")

set(SOURCES 
	"${PROJECT_SOURCE_DIR}/src/misc/file.c"
	"${PROJECT_SOURCE_DIR}/src/nhgui.c"
)


add_subdirectory(example/simple)

