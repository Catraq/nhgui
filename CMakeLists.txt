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
	"${PROJECT_SOURCE_DIR}/src/shader.c"
	"${PROJECT_SOURCE_DIR}/src/surface.c"
	"${PROJECT_SOURCE_DIR}/src/context.c"
	"${PROJECT_SOURCE_DIR}/src/result.c"
	"${PROJECT_SOURCE_DIR}/src/common.c"
	"${PROJECT_SOURCE_DIR}/src/icon/menu.c"
	"${PROJECT_SOURCE_DIR}/src/object/radio_button.c"
	"${PROJECT_SOURCE_DIR}/src/object/font.c"
)


add_subdirectory(example/simple)

