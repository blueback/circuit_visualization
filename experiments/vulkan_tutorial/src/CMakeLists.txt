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
add_executable(VulkanTest main.cpp)


##################################################
# Add include directories for executable
#
target_include_directories(VulkanTest
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(VulkanTest
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})


##################################################
# Define link libraries
#
set(VULKANTEST_LIBRARIES
    "$<$<CONFIG:Debug>:${LIB_PTHREAD_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_PTHREAD_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_DYNAMIC_LINK_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_DYNAMIC_LINK_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_VULKAN_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_VULKAN_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_GLFW_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_GLFW_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_X11_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_X11_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_XXF86VM_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_XXF86VM_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_XRANDR_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_XRANDR_OPTIONS}>"
    "$<$<CONFIG:Debug>:${LIB_XI_OPTIONS}>"
    "$<$<CONFIG:Release>:${LIB_XI_OPTIONS}>"
)


####################################################################
# link libraries
#
target_link_libraries(VulkanTest
	PRIVATE
    ${VULKANTEST_LIBRARIES})
