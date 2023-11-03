#include <ebb/core/window.hpp>
#include <ebb/internal/internals.hpp>
#include <ebb/transform.hpp>
#include <ebb/object.hpp>
#include <ebb/mesh.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/render/blinn_phong.hpp>
#include <ebb/render/mesh_renderer.hpp>
#include <ebb/error.hpp>
#include <glad/glad.h>
#include <stdio.h>

Ebb::Window *win;
Ebb::Object *suzanne;
Ebb::Mesh *mesh;
Ebb::Shader *shader;

void frame_callback() {
        // Close the window when ESC is pressed
    if (glfwGetKey(win->get_window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        win->close();
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    suzanne->get_child<Ebb::MeshRenderer>()->draw();
}

int main(int argc, char **argv) {
        // Create the window
    win = new Ebb::Window(800, 800, "Test mesh", frame_callback);

        // Setup GLAD. This will be automated eventually
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error(true, "Failed to load GLAD, please check your video drivers.");
    }

    mesh = new Ebb::Mesh("models/suzanne.ebbm");
    shader = new Ebb::Shaders::BlinnPhong(
        glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(.9f), glm::vec3(.1f),
        1.0f, 1.0f, glm::vec3(.9f), glm::vec3(.95f), 128.0f
    );
    suzanne = new Ebb::Object(nullptr);
    suzanne->set_transform(Ebb::Transform(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 180.0, 0.0)));
    Ebb::MeshRenderer *renderer = new Ebb::MeshRenderer(suzanne, shader, mesh);

    Ebb::Internals::currentCameraPos = glm::vec3(0.0, 0.0, -10000.0);

        // Run the window. No frame count is specified, the window will run forever
    win->run();
    return 0;
}