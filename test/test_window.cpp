#include <ebb/core/window.hpp>
#include <ebb/error.hpp>
#include <glad/glad.h>
#include <stdio.h>

Ebb::Window *win;
void frame_callback() {
        // Close the window when ESC is pressed
    if (glfwGetKey(win->get_window(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        win->close();
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char **argv) {
        // Create the window
    win = new Ebb::Window(800, 600, "Test window", frame_callback);

        // Setup GLAD. This will be automated eventually
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error(true, "Failed to load GLAD, please check your video drivers.");
    }

        // Run the window. No frame count is specified, the window will run forever
    win->run();
    return 0;
}