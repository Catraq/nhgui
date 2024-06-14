set(TEMPLATE list)

set(CMAKE_C_FLAGS "-Wall")

add_executable(${TEMPLATE} main.c)
 
set_property(TARGET ${TEMPLATE} PROPERTY C_STANDARD 11)

target_link_libraries(${TEMPLATE} nhgui)

