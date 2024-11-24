##################################################
# Define sources for animation demo
#
set(ANIMATION_DEMO_SOURCES
    animation_demo.cpp
    animation_demo_self_test.cpp)


##################################################
# Add library for animation demo
#
add_library(animation_demo
	STATIC
    ${ANIMATION_DEMO_SOURCES})


##################################################
# Set PIC for library for animation demo
#
set_target_properties(animation_demo
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for animation demo
#
target_include_directories(animation_demo
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(animation_demo
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(animation_demo
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(animation_demo
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for animation demo
#
target_compile_options(
    animation_demo PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define animation demo link libraries
#
set(ANIMATION_DEMO_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:raylib>"
    "$<$<CONFIG:Release>:raylib>"
)


##################################################
# link libraries
#
target_link_libraries(animation_demo
	PRIVATE
    ${ANIMATION_DEMO_LINK_LIBRARIES})
