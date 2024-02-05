cmake_minimum_required(VERSION 3.11)
project(Ebb)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
)

FetchContent_MakeAvailable(glm)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Set the path to your library folder
set(LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Get all the library source files
file(GLOB LIB_SOURCES "${LIBRARY_DIR}/*.cpp")

# Add a library target
add_library(ebb STATIC ${LIB_SOURCES})

# Enable CTest to automatically discover tests
enable_testing()

# Set the path to your test folder
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Get all the test source files
file(GLOB_RECURSE TEST_SOURCES "${TEST_DIR}/*.cpp" "${TEST_DIR}/*.c")

foreach(test_source ${TEST_SOURCES})
    # Get the base name of the test source file (without extension)
    get_filename_component(test_name ${test_source} NAME_WE)

    # Add an executable target for the test source file
    add_executable(${test_name} ${test_source})

	set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/)
	target_link_libraries(${test_name} PRIVATE glm::glm)
	target_link_libraries(${test_name} PRIVATE ebb)
	target_link_libraries(${test_name} PRIVATE glfw)
	target_link_libraries(${test_name} PRIVATE GL)
	target_link_libraries(${test_name} PRIVATE glad)
	target_link_libraries(${test_name} PRIVATE GLU)
endforeach()