##################################################
# Define sources for circuit solver
#
set(CIRCUIT_SOLVER_SOURCES
    circuit_solver.cpp
    circuit_solver_self_test.cpp)


##################################################
# Add library for circuit solver
#
add_library(circuit_solver
	STATIC
    ${CIRCUIT_SOLVER_SOURCES})


##################################################
# Set PIC for library for circuit solver
#
set_target_properties(circuit_solver
	PROPERTIES
	POSITION_INDEPENDENT_CODE ON)


##################################################
# Add include directories for circuit solver
#
target_include_directories(circuit_solver
	AFTER PRIVATE
	${CMAKE_SOURCE_DIR}/include)
target_include_directories(circuit_solver
	AFTER PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR})


##################################################
# Compiler options for circuit solver
#
target_compile_options(
    circuit_solver PRIVATE 
    "$<$<CONFIG:Debug>:>"
    "$<$<CONFIG:Release>:>"
)


##################################################
# Define circuit solver link libraries
#
set(CIRCUIT_SOLVER_LINK_LIBRARIES
	"$<$<CONFIG:Debug>:>"
	"$<$<CONFIG:Release>:>"
)


##################################################
# link libraries
#
target_link_libraries(circuit_solver
	PRIVATE
    ${CIRCUIT_SOLVER_LINK_LIBRARIES})
