cmake_minimum_required(VERSION 3.11)
project(Ebb)

include_directories(../include/)
add_executable(main run/main.cpp)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git # just use latest version
)

FetchContent_MakeAvailable(glm)

target_link_libraries(main PRIVATE glm::glm)