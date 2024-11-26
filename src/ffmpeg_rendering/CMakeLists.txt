##################################################
# Define sources for ffmpeg rendering
#
set(FFMPEG_RENDERING_SOURCES
    ffmpeg_linux.cpp)


##################################################
# Add library for ffmpeg rendering
#
add_library(ffmpeg_rendering
	STATIC
    ${FFMPEG_RENDERING_SOURCES})


##################################################
# Set PIC for library for ffmpeg rendering
#
set_target_properties(ffmpeg_rendering
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for ffmpeg rendering
#
target_include_directories(ffmpeg_rendering
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(ffmpeg_rendering
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ffmpeg_rendering
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(ffmpeg_rendering
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for ffmpeg rendering
#
target_compile_options(
    ffmpeg_rendering PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define ffmpeg rendering link libraries
#
set(FFMPEG_RENDERING_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# link libraries
#
target_link_libraries(ffmpeg_rendering
	PRIVATE
    ${FFMPEG_RENDERING_LINK_LIBRARIES})
