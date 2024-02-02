#include <ebb/window.hpp>
#include <ebb/util/file.hpp>

Ebb::Window::Window(int width, int height, const std::string& title = "Ebb Engine") : title(title) {
    Ebb::Window::setup();
    this->_win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (this->_win == nullptr) {
        // TODO: show error message or smth
        return;
    }
    glfwMakeContextCurrent(this->_win);

    this->width  =  width;
    this->height = height;
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

void Ebb::Window::teardown() {
    if (!Ebb::Window::initialized) return;
    Ebb::Window::initialized = false;
    glfwTerminate();
}

/**
 * |width: i32| |height: i32| |title: nts/c-str|
*/
void Ebb::Window::save(FILE *file) {
    fwrite(&this->width, 1, sizeof(this->width), file);
    fwrite(&this->height, 1, sizeof(this->height), file);

    Ebb::Util::Files::writeNts(file, this->title);
}

void Ebb::Window::load(FILE *file) {
    fread(&this->width, 1, sizeof(this->width), file);
    fread(&this->height, 1, sizeof(this->height), file);

    Ebb::Util::Files::readNts(file, this->title);
}