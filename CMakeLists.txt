cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# libcross2dui version
set(CROSS2DUI_VERSION_MAJOR 1)
set(CROSS2DUI_VERSION_MINOR 0)

# delete cmake cache folder before changing this options
option(BUILD_SFML "Build with SFML support (hardware rendering, fast)" OFF)
option(BUILD_SDL1 "Build with SDL1 support (software, osmesa) support" OFF)
option(BUILD_SDL1_GL "Build with SDL1 support (hardware rendering, fast, needs BUILD_SDL1=ON)" OFF)
option(BUILD_SDL2 "Build with SDL2 support (software)" ON)
option(BUILD_SDL2_GL "Build with SDL2 support (hardware rendering, fast, needs BUILD_SDL2=ON)" ON)
option(BUILD_PSP2 "Build with PSP2 support (hardware rendering, fast)" OFF)
option(BUILD_PS3 "Build with PS3 support (hardware rendering, fast)" OFF)
option(BUILD_3DS "Build with 3DS support" OFF)
option(BUILD_NX "Build with NX (SWITCH) support" OFF)
option(BUILD_NX_GL "Build with NX support (software, osmesa)" OFF)

option(BUILD_PFBA "Build with PFBA support" OFF)
option(BUILD_SNES9X "Build with SNES9X support" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG true CACHE BOOL "Debug build")
endif ()

# setup toolchain
include(libcross2d/Toolchain.cmake)
enable_language(ASM)

# add cross2d library. If you want to port libcross2dui to another platform,
# you should (may) only need to port this library.
add_subdirectory(libcross2d)

project(cross2dui)

#################
# libcross2dui
#################
set(CROSS2DUI_INCLUDE include)
file(GLOB CROSS2DUI_SOURCE source/*.c*)

##############
# libconfig
##############
set(LIBCONFIG_INCLUDE libconfig)
file(GLOB LIBCONFIG_SOURCE
        libconfig/grammar.c
        libconfig/libconfig.c
        libconfig/scanctx.c
        libconfig/scanner.c
        libconfig/strbuf.c
        )

# flags
set(FLAGS
        -D__CROSS2DUI_VERSION_MAJOR__=${CROSS2DUI_VERSION_MAJOR}
        -D__CROSS2DUI_VERSION_MINOR__=${CROSS2DUI_VERSION_MINOR}
        )

if (BUILD_PFBA)
    #################
    # PFBA
    #################
    list(APPEND FLAGS -D__PFBA__)
elseif (BUILD_SNES9X)
    ##############
    # SNES9X
    ##############
    list(APPEND FLAGS -D__SNES9X__)
endif (BUILD_PFBA)

if (BUILD_PSP2)
    #################
    # PSP2 (ps vita)
    #################
elseif (BUILD_PS3)
    ##############
    # PS3
    ##############
elseif (BUILD_3DS)
    ##############
    # 3DS
    ##############
elseif (BUILD_NX)
    ##############
    # NX (SWITCH)
    ##############
elseif (BUILD_SDL2)
    ##############
    # SDL2
    ##############
elseif (BUILD_SDL1)
    ##############
    # SDL1
    ##############
elseif (BUILD_SFML)
    ##############
    # SFML
    ##############
endif (BUILD_PSP2)

##########################
# libcross2dui
##########################
add_library(${PROJECT_NAME} ${CROSS2DUI_SOURCE} ${LIBCONFIG_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CROSS2DUI_INCLUDE} ${LIBCONFIG_INCLUDE})
target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11 -fpermissive>)
target_link_libraries(${PROJECT_NAME} cross2d)

#########################
# libcross2dui (pfba) test
#########################
set(PFBA_TEST_INCLUDE pfba_test)
file(GLOB PFBA_TEST_SOURCE pfba_test/*.c*)

add_executable(${PROJECT_NAME}.elf ${PFBA_TEST_SOURCE})
target_include_directories(${PROJECT_NAME}.elf  PRIVATE ${PFBA_TEST_INCLUDE})
target_link_libraries(${PROJECT_NAME}.elf ${PROJECT_NAME})

#####################
# PSP2 (vita) vpk
#####################
if (BUILD_PSP2)
endif (BUILD_PSP2)

#####################
# PS3
#####################
if (BUILD_PS3)
endif (BUILD_PS3)

#####################
# CTR (3DS) target
#####################
if (BUILD_3DS)
endif (BUILD_3DS)

#####################
# NX (SWITCH) target
#####################
if (BUILD_NX)
endif (BUILD_NX)