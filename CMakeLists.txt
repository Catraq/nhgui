cmake_minimum_required(VERSION 3.0.2)
project(rl_gui)



include_directories("${PROJECT_SOURCE_DIR}/src/")
include_directories("${PROJECT_SOURCE_DIR}/include/")

set(SOURCES 
	"${PROJECT_SOURCE_DIR}/src/rl_gui.c"
	"${PROJECT_SOURCE_DIR}/src/rl_gui_glfw.c"
)

add_library(rl_gui SHARED ${SOURCES})

find_package(GLEW REQUIRED)
target_include_directories(rl_gui PUBLIC ${GLEW_INCLUDE_DIRS})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_include_directories(rl_gui PUBLIC ${OpenGL_INCLUDE_DIR})

find_package(glfw3 3.0 REQUIRED)

find_package(Freetype REQUIRED)
target_include_directories(rl_gui PUBLIC ${FREETYPE_INCLUDE_DIRS})

#Dependencies library 
target_link_libraries(rl_gui PUBLIC m)
target_link_libraries(rl_gui PUBLIC ${GLEW_LIBRARIES})
target_link_libraries(rl_gui PUBLIC glfw)
target_link_libraries(rl_gui PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(rl_gui PUBLIC ${FREETYPE_LIBRARIES})

target_include_directories(rl_gui PUBLIC "include/")




add_subdirectory(example/list)
add_subdirectory(example/simple)


