#include <ebb/window.hpp>

Ebb::Window::Window(int width, int height, const std::string& title = "Ebb Engine") {
    this->_win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (this->_win == nullptr) {
        // TODO: show error message or smth
        return;
    }
    glfwMakeContextCurrent(this->_win);
}

void Ebb::Window::update() {
    if (this->_win == nullptr) return;
    if (glfwWindowShouldClose(this->_win)) {
        glfwDestroyWindow(this->_win);
        return;
    }

    glfwSwapBuffers(this->_win);
    glfwPollEvents(); // TODO: actually do stuff with the events
}

bool Ebb::Window::initialized = false;
void Ebb::Window::setup() {
    if (Ebb::Window::initialized) return;
    Ebb::Window::initialized = true;
    if (!glfwInit()) {
        printf("GLFW failed to initialize.\n");
        exit(1);
    }
}