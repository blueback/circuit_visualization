##################################################
# Define sources for standard defs
#
set(STANDARD_DEFS_SOURCES
    standard_defs.cpp)


##################################################
# Add library for standard defs
#
add_library(standard_defs
	STATIC
    ${STANDARD_DEFS_SOURCES})


##################################################
# Set PIC for library for standard defs
#
set_target_properties(standard_defs
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for standard defs
#
target_include_directories(standard_defs
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(standard_defs
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(standard_defs
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(standard_defs
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for standard defs
#
target_compile_options(
    standard_defs PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define standard defs link libraries
#
set(STANDARD_DEFS_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# link libraries
#
target_link_libraries(standard_defs
	PRIVATE
    ${STANDARD_DEFS_LINK_LIBRARIES})
