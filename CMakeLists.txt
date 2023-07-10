cmake_minimum_required (VERSION 3.10)
project(Bspline)

set (Bspline_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (Bspline_INCLUDE_DIR ${Bspline_BASE_DIR}/inc)
set (Bspline_SRC_DIR ${Bspline_BASE_DIR}/src)
set (Bspline_SHADER_DIR ${Bspline_BASE_DIR}/shader)
set (THIRD_BASE_DIR ${Bspline_BASE_DIR}/third)
set (THIRD_INCLUDE_DIR ${THIRD_BASE_DIR}/inc)
set (THIRD_SRC_DIR ${THIRD_BASE_DIR}/src)
set (THIRD_LIB_DIR ${THIRD_BASE_DIR}/lib)

file(GLOB SHADERS "${Bspline_SHADER_DIR}/*")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
	${Bspline_INCLUDE_DIR}
	${THIRD_INCLUDE_DIR})

aux_source_directory(${Bspline_SRC_DIR} Bspline_SRCS)
aux_source_directory(${THIRD_SRC_DIR} THIRD_SRCS)

add_executable(Bspline ${Bspline_SRCS} ${THIRD_SRCS})

set (THIRD_LIBS ${THIRD_LIB_DIR}/glfw3.lib;opengl32.lib)
target_link_libraries(Bspline ${THIRD_LIBS})