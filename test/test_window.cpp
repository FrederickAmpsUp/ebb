#include <ebb/core/window.hpp>
#include <stdio.h>

Ebb::Window *win;
void frame_callback() {
        // Close the window when ESC is pressed
    if (win->key_pressed(GLFW_KEY_ESCAPE)) {
        win->close();
    }

    win->clear(glm::vec3(1.0f));
}

int main(int argc, char **argv) {
        // Create the window
    win = new Ebb::Window(800, 600, "Test window", frame_callback);

        // Run the window. No frame count is specified, the window will run forever
    win->run();
    return 0;
}