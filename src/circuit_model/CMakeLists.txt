##################################################
# Define sources for circuit model
#
set(CIRCUIT_MODEL_SOURCES
    circuit_model.cpp
    circuit_model_self_test.cpp)


##################################################
# Add library for circuit model
#
add_library(circuit_model
	STATIC
    ${CIRCUIT_MODEL_SOURCES})


##################################################
# Set PIC for library for circuit model
#
set_target_properties(circuit_model
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for circuit model
#
target_include_directories(circuit_model
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(circuit_model
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(circuit_model
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/third_party/usr/local/include)


##################################################
# Append link directories
#
target_link_directories(circuit_model
    PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/usr/local/lib)


##################################################
# Compiler options for circuit model
#
target_compile_options(
    circuit_model PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define circuit model link libraries
#
set(CIRCUIT_MODEL_LINK_LIBRARIES
    "$<$<CONFIG:Debug>:raylib>"
    "$<$<CONFIG:Release>:raylib>"
)


##################################################
# link libraries
#
target_link_libraries(circuit_model
	PRIVATE
    ${CIRCUIT_MODEL_LINK_LIBRARIES})
