#include <ebb/core/window.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <ebb/error.hpp>

static void _window_size_callback(GLFWwindow *window, int width, int height) {
    Ebb::Window* instance = reinterpret_cast<Ebb::Window*>(glfwGetWindowUserPointer(window));
    instance->_window_resize_callback(window, width, height);
}

static void _glfw_init() {
    if (!glfwInit()) {
        Ebb::runtime_error(true, "Failed to initialize GLFW.\n");
    }
}

Ebb::Window::Window(int width, int height, const char *title) {
    _glfw_init();
    this->width = width;
    this->height = height;

    this->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (this->window == nullptr) {
        Ebb::runtime_error(true, "Failed to create GLFW window.\n");
    }

    glfwSetWindowUserPointer(this->window, this);
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error(true, "Failed to initialize GLAD.\n");
    }
    
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (glfwExtensionSupported("GL_ARB_multisample")) {
        glEnable(GL_MULTISAMPLE);
        glfwWindowHint(GLFW_SAMPLES, 4);
    }


    glfwSetWindowSizeCallback(this->window, _window_size_callback);

    this->render_loop_callback = nullptr;
}

Ebb::Window::Window(int width, int height, const std::string& title) : Window(width, height, title.c_str()) {}

Ebb::Window::Window(int width, int height, const std::string& title, RenderLoopCallback callback) : Window(width, height, title.c_str()) {
    this->render_loop_callback = callback;
}

Ebb::Window::Window(int width, int height, const char *title, RenderLoopCallback callback) : Window(width, height, title) {
    this->render_loop_callback = callback;
}

Ebb::Window::Window(const char *title) {
    _glfw_init();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int width = mode->width;
    int height = mode->height;
        
    this->width = width;
    this->height = height;

    this->window = glfwCreateWindow(width, height, title, monitor, NULL);
    if (this->window == nullptr) {
        Ebb::runtime_error(true, "Failed to create GLFW window.\n");
    }

    glfwSetWindowUserPointer(this->window, this);
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Ebb::runtime_error(true, "Failed to initialize GLAD.\n");
    }

    glViewport(0, 0, width, height);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (glfwExtensionSupported("GL_ARB_multisample")) {
        glEnable(GL_MULTISAMPLE);
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

    glfwSetWindowSizeCallback(this->window, _window_size_callback);

    this->render_loop_callback = nullptr;
}
Ebb::Window::Window(const std::string& title) : Window(title.c_str()) {}
Ebb::Window::Window(const std::string& title, RenderLoopCallback callback) : Window(title) {
    this->render_loop_callback = callback;
}

Ebb::Window::Window(const char *title, RenderLoopCallback callback) : Window(title) {
    this->render_loop_callback = callback;
}

void Ebb::Window::_window_resize_callback(GLFWwindow *window, int width, int height) {
    glfwMakeContextCurrent(window);

    glViewport(0, 0, width, height);

    this->width = width;
    this->height = height;
}

void Ebb::Window::run(unsigned long long frames) {
    for (; frames > 0 && !glfwWindowShouldClose(this->window); --frames) {
        step();
    }
}

void Ebb::Window::step() {
    glfwMakeContextCurrent(this->window);
    glfwSwapBuffers(window);
    glfwPollEvents();
    if (this->render_loop_callback != nullptr) this->render_loop_callback();
}

void Ebb::Window::make_active() {
    glfwMakeContextCurrent(this->window);
}

void Ebb::Window::close() {
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}