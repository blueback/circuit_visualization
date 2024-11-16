##################################################
# Define src_root include directory
#
set(SRC_ROOT_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/src)


##################################################
# Subdirectories for src
#
add_subdirectory(circuit_solver)
#add_subdirectory(subdir1)
#add_subdirectory(subdir2)


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


####################################################################
# link libraries
#
set(CIRCUIT_VIS_LIBRARIES 
	circuit_solver)
target_link_libraries(circuit_vis
	PRIVATE
    ${CIRCUIT_VIS_LIBRARIES})
