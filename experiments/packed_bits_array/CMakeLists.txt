cmake_minimum_required(VERSION 3.25)


####################################################################
# Disable in-source builds to prevent source tree corruption.
#
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


####################################################################
# Project Name
#
project(tmp)


####################################################################
# Build only for gcc-11 version
#
if ("${CMAKE_C_COMPILER_VERSION}" VERSION_LESS "11.1.0")
    message(FATAL_ERROR "FATAL : mininum gcc version should be 11.1.0.")
endif()


####################################################################
# Set coverage flags for coverage build
#
if(${COV_BUILD} MATCHES true)
    message("Coverage Enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
else()
    message("Coverage Disabled")
endif()


####################################################################
# Set compile flags
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib)
endif()
if (NOT DEFINED CMAKE_SCRIPT_OUTPUT_DIRECTORY)
	set(CMAKE_SCRIPT_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/scripts)
endif()


####################################################################
# Set intel compile flags
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                    -march=raptorlake \
                    -masm=intel \
                    -m64")
                    ################################################
                    # TODO: this flag not working with clangd(lsp)
                    ################################################
                    # -mtune=intel
                    ################################################


####################################################################
# Set vector instructions compile flags
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                    -O3 \
                    -mmmx \
                    -mavx \
                    -mavx2\
                    -mavxvnni\
                    -mfma \
                    -mbmi \
                    -mbmi2 \
                    -madx \
                    -mrdrnd")


####################################################################
# Build only for linux
#
if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
    message("System name is Linux")
    add_definitions(-DPROJECT_SYSTEM="Linux")
else()
    message(FATAL_ERROR "System is Not Linux")
endif()


####################################################################
# Library for flex, mathematics, pthreads, dynamic linking loader
#
set(LIB_FLEX_OPTIONS
	"-lfl")
set(LIB_MATH_OPTIONS
	"-lm")
set(LIB_PTHREAD_OPTIONS
	"-lpthread")
set(LIB_DYNAMIC_LINK_OPTIONS
	"-ldl")


####################################################################
# Build type (debug or release)
#
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    message("Debug Build")
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    message("Release Build")
else()
    message(FATAL_ERROR "FATAL: Invalid build type.")
endif()


##################################################
# Subdirectory src for root
#
add_subdirectory(src)
