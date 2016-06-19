cmake_minimum_required(VERSION 2.6)
set( PROJECT_NAME "r2t2_dsp" )

project(r2t2_dsp CXX C ASM)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
	${CMAKE_CURRENT_SOURCE_DIR}/freertos/portable/GCC/ARM_CA9
    ${CMAKE_CURRENT_SOURCE_DIR}/r2t2
)

add_definitions(-D USE_AMP=1) 

#set (comp_defs "-Wall -Werror")
set (comp_defs "-Wall")

set ( CMAKE_ASM_FLAGS "-g -D USE_AMP=1 -march=armv7-a -mfloat-abi=soft -mfpu=neon" )
set ( CMAKE_ASM_FLAGS_DEBUG "-D DEBUG" )

set( CMAKE_C_CXX_FLAGS_RELEASE "-O2 -D NDEBUG -D RELEASE " )
set( CMAKE_C_CXX_FLAGS_DEBUG   "-g -D DEBUG -D __DEBUG__ -U __RELEASE__ -U RELEASE" )

set( CMAKE_C_FLAGS           "${comp_defs}" )
set( CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_CXX_FLAGS_RELEASE}" )
set( CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_CXX_FLAGS_DEBUG}" )

set( CMAKE_CXX_FLAGS         "${comp_defs} ${comp_defs_cpp}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_CXX_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_CXX_FLAGS_DEBUG}" )


set( CMAKE_EXE_LINKER_FLAGS ${LIB_DIRS} " -Wl,-T -Wl,../lscript.ld  -Wl,--start-group r2t2/libr2t2.a bsp/libbsp.a freertos/libfreertos.a -Wl,--end-group  -Wl,--start-group,-lgcc,-lc,--end-group  ")
#set( CMAKE_EXE_LINKER_FLAGS ${LIB_DIRS} " -Wl,-T -Wl,../lscript.ld   -Wl,--start-group,-lgcc,-lc,--end-group  ")
set( CMAKE_EXE_LINKER_FLAGS_DEBUG "")
set( CMAKE_EXE_LINKER_FLAGS_RELEASE "")

add_subdirectory(bsp)
add_subdirectory(freertos)
add_subdirectory(r2t2)

add_executable(${PROJECT_NAME} dummy.c)
target_link_libraries(${PROJECT_NAME} r2t2 bsp freertos)


