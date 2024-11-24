##################################################
# Define sources for circuit animator
#
set(CIRCUIT_ANIMATOR_SOURCES
    circuit_animator.cpp)


##################################################
# Add library for circuit animator
#
add_library(circuit_animator
	STATIC
    ${CIRCUIT_ANIMATOR_SOURCES})


##################################################
# Set PIC for library for circuit animator
#
set_target_properties(circuit_animator
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for circuit animator
#
target_include_directories(circuit_animator
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(circuit_animator
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(circuit_animator
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(circuit_animator
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for circuit animator
#
target_compile_options(
    circuit_animator PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define circuit animator link libraries
#
set(CIRCUIT_ANIMATOR_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:circuit_model>"
    "$<$<CONFIG:Release>:circuit_model>"
    "$<$<CONFIG:Debug>:raylib>"
    "$<$<CONFIG:Release>:raylib>"
)


##################################################
# link libraries
#
target_link_libraries(circuit_animator
	PRIVATE
    ${CIRCUIT_ANIMATOR_LINK_LIBRARIES})
