##################################################
# Define src_root include directory
#
set(SRC_ROOT_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/src)


##################################################
# Subdirectories for src
#
#add_subdirectory(subdir1)


##################################################
# Create executable
#
add_executable(tmp tmp.cpp)


##################################################
# Add include directories for executable
#
target_include_directories(tmp
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(tmp
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})


##################################################
# Define link libraries
#
set(TMP_LIBRARIES
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


####################################################################
# link libraries
#
target_link_libraries(tmp
	PRIVATE
    ${TMP_LIBRARIES})
