#include <ebb/core/window.hpp>
#include <ebb/mesh.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/render/mesh_renderer.hpp>
#include <ebb/error.hpp>
#include <glad/glad.h>
#include <stdio.h>

Ebb::Window *win;
Ebb::MeshRenderer *renderer;
Ebb::Mesh *mesh;
Ebb::Shader *shader;

void frame_callback() {
        // Close the window when ESC is pressed
    if (glfwGetKey(win->get_window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        win->close();
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer->draw();
}

int main(int argc, char **argv) {
        // Create the window
    win = new Ebb::Window(800, 600, "Test mesh", frame_callback);

        // Setup GLAD. This will be automated eventually
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error(true, "Failed to load GLAD, please check your video drivers.");
    }

    mesh = new Ebb::Mesh("models/suzanne.ebbm");
    shader = new Ebb::Shader(R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 objectMatrix;

varying vec3 normal;
void main() {
    gl_Position = vec4(aPos, 1.0) * objectMatrix;
    normal = aNorm;
}
    )", R"(
#version 330 core
varying vec3 normal;

void main() {
    gl_FragColor = vec4(normal, 1.0);
}
    )");
    renderer = new Ebb::MeshRenderer(nullptr, shader, mesh);

        // Run the window. No frame count is specified, the window will run forever
    win->run();
    return 0;
}