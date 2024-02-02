#include <ebb/node.hpp>
#include <ebb/window.hpp>
#include <stdio.h>

using Node = Ebb::Node;
int main(int argc, char **argv) {
    Node *root = new Node;
    Node *win = new Ebb::WindowManager(root, new Ebb::Window(800,600, "Test window"));

    FILE *file = fopen("tests/window.ebb", "wb");
    if (file == nullptr) {
        return -1;
    }
    root->save(file);

    root->print();
    putchar('\n');

    Ebb::NodeTreeManager manager(root);
    manager.run();

    Ebb::Window::teardown();

    return 0;
}