##################################################
# Define src_root include directory
#
set(SRC_ROOT_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/src)


##################################################
# Subdirectories for src
#
add_subdirectory(circuit_model)
add_subdirectory(circuit_animator)
add_subdirectory(circuit_solver)
add_subdirectory(raylib_probe)
add_subdirectory(animation_demo)
add_subdirectory(ffmpeg_rendering)


##################################################
# Create executable
#
add_executable(circuit_vis main.cpp)


##################################################
# Add include directories for executable
#
target_include_directories(circuit_vis
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(circuit_vis
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})


##################################################
# Append link directories
#
target_link_directories(circuit_vis
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Define link libraries
#
set(CIRCUIT_VIS_LIBRARIES
    "$<$<CONFIG:Debug>:circuit_solver>"
    "$<$<CONFIG:Release>:circuit_solver>"
    "$<$<CONFIG:Debug>:raylib_probe>"
    "$<$<CONFIG:Release>:raylib_probe>"
    "$<$<CONFIG:Debug>:animation_demo>"
    "$<$<CONFIG:Release>:animation_demo>"
)


####################################################################
# link libraries
#
target_link_libraries(circuit_vis
	PRIVATE
    ${CIRCUIT_VIS_LIBRARIES})
