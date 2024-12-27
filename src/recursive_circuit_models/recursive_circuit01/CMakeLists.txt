##################################################
# Define sources for recursive circuit 01
#
set(RECURSIVE_CIRCUIT01_SOURCES
    recursive_circuit01.cpp
    recursive_circuit01_self_test.cpp)


##################################################
# Add library for recursive circuit 01
#
add_library(recursive_circuit01
	STATIC
    ${RECURSIVE_CIRCUIT01_SOURCES})


##################################################
# Set PIC for library for recursive circuit 01
#
set_target_properties(recursive_circuit01
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for recursive circuit 01
#
target_include_directories(recursive_circuit01
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(recursive_circuit01
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(recursive_circuit01
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(recursive_circuit01
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for recursive circuit 01
#
target_compile_options(
    recursive_circuit01 PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define recursive circuit 01 link libraries
#
set(RECURSIVE_CIRCUIT01_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:raylib>"
    "$<$<CONFIG:Release>:raylib>"
)


##################################################
# link libraries
#
target_link_libraries(recursive_circuit01
	PRIVATE
    ${RECURSIVE_CIRCUIT01_LINK_LIBRARIES})
