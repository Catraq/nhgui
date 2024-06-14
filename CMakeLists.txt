cmake_minimum_required(VERSION 3.0.2)
project(nhgui)



include_directories("${PROJECT_SOURCE_DIR}/src/")
include_directories("${PROJECT_SOURCE_DIR}/include/")

set(SOURCES 
	"${PROJECT_SOURCE_DIR}/src/misc/file.c"
	"${PROJECT_SOURCE_DIR}/src/nhgui.c"
	"${PROJECT_SOURCE_DIR}/src/nhgui_glfw.c"
)

add_library(nhgui SHARED ${SOURCES})

find_package(GLEW REQUIRED)
target_include_directories(nhgui PUBLIC ${GLEW_INCLUDE_DIRS})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_include_directories(nhgui PUBLIC ${OpenGL_INCLUDE_DIR})

find_package(glfw3 3.0 REQUIRED)

find_package(Freetype REQUIRED)
target_include_directories(nhgui PUBLIC ${FREETYPE_INCLUDE_DIRS})

#Dependencies library 
target_link_libraries(nhgui PUBLIC m)
target_link_libraries(nhgui PUBLIC ${GLEW_LIBRARIES})
target_link_libraries(nhgui PUBLIC glfw)
target_link_libraries(nhgui PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(nhgui PUBLIC ${FREETYPE_LIBRARIES})

target_include_directories(nhgui PUBLIC "include/")




add_subdirectory(example/list)


