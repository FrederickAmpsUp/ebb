#include <ebb/window.hpp>

int main(int argc, char **argv) {
    Ebb::Window::setup();
    Ebb::Window *win = new Ebb::Window(800,600, "Test Window");

    while (win->open()) {
        win->update();
    }

    return 0;
}