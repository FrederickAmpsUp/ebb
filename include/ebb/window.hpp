#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace Ebb {
class Window {
public:
    Window(int width, int height, const std::string& title);

    void update();
    bool open() { return this->_win != nullptr && !glfwWindowShouldClose(this->_win); }

    static void setup();
private:
    GLFWwindow *_win;
    static bool initialized;
};
};