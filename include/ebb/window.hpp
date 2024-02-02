#pragma once

#include <GLFW/glfw3.h>
#include <ebb/serializable.hpp>
#include <ebb/node.hpp>
#include <string>
#include <stdio.h>

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

    int  width() { return _width ; }
    int height() { return _height; }

    std::string title() { return _title; }
private:
    GLFWwindow *_win;
    static bool initialized;

    int _width, _height;
    std::string _title;
};

class WindowManager : public Ebb::Node {
public:
    WindowManager(Ebb::Node *parent, Ebb::Window *win) : Node(parent), win(win) {}

    void update() {
        if (this->win != nullptr) {
            if (!this->win->open()) this->findRoot()->active = false; // disable the entire nodetree, closing the program
            this->win->update();
        }

        this->Node::update(); // back up the inheritance tree :)
    }

    void print() {
        printf("WindowManager{");
        this->Node::print();
        printf(", %d, %d, \"%s\"}", win->width(), win->height(), win->title().c_str());
    }
private:
    Ebb::Window *win;
};
};