#pragma once

#include <GLFW/glfw3.h>
#include <ebb/serializable.hpp>
#include <string>

namespace Ebb {
class Window : public Ebb::Serializable {
public:
    Window(int width, int height, const std::string& title);

    void update();
    bool open() { return this->_win != nullptr && !glfwWindowShouldClose(this->_win); }

    static void setup();
    static void teardown();

    void save(FILE *file) override;
    void load(FILE *file) override;
private:
    GLFWwindow *_win;
    static bool initialized;

    int width, height;
    std::string title;
};
};