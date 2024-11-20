##################################################
# Define sources for raylib probe
#
set(RAYLIB_PROBE_SOURCES
    raylib_probe.cpp
    raylib_probe_self_test.cpp)


##################################################
# Add library for raylib probe
#
add_library(raylib_probe
	STATIC
    ${RAYLIB_PROBE_SOURCES})


##################################################
# Set PIC for library for raylib probe
#
set_target_properties(raylib_probe
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for raylib probe
#
target_include_directories(raylib_probe
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(raylib_probe
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(raylib_probe
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(raylib_probe
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for raylib probe
#
target_compile_options(
    raylib_probe PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define raylib probe link libraries
#
set(RAYLIB_PROBE_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:raylib>"
    "$<$<CONFIG:Release>:raylib>"
)


##################################################
# link libraries
#
target_link_libraries(raylib_probe
	PRIVATE
    ${RAYLIB_PROBE_LINK_LIBRARIES})
