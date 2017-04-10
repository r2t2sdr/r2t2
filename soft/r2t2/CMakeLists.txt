cmake_minimum_required(VERSION 2.6)
set( PROJECT_NAME "r2t2" )

project(r2t2 CXX C ASM)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
	.
	)

# set (comp_defs "-Wall -Werror ")
set (comp_defs "-Wall -lm -g")

set( CMAKE_C_CXX_FLAGS_RELEASE "-O2 -D NDEBUG -D RELEASE " )
set( CMAKE_C_CXX_FLAGS_DEBUG   "-g -D DEBUG -D __DEBUG__ -U __RELEASE__ -U RELEASE" )

set( CMAKE_C_FLAGS           "-D_GNU_SOURCE ${comp_defs}" )
set( CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_CXX_FLAGS_RELEASE}" )
set( CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_CXX_FLAGS_DEBUG}" )

set( CMAKE_CXX_FLAGS         "-std=gnu++11  ${comp_defs} ${comp_defs_cpp}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_CXX_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_CXX_FLAGS_DEBUG}" )


add_executable(${PROJECT_NAME}
	reg.c
	spi.c
    i2c.c
	si5344_init.c
	socket.c
	main.c
	)
