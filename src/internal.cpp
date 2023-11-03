#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ebb/error.hpp>

void setup_glad() {
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error("Failed to load GLAD, please check your video drivers.");
    }
}