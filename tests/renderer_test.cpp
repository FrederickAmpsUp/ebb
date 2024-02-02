#include <ebb/node.hpp>
#include <ebb/window.hpp>
#include <ebb/renderer.hpp>
#include <stdio.h>

using Node = Ebb::Node;
int main(int argc, char **argv) {
    Node *root = new Node;
    Ebb::NodeTreeManager manager(root);

    Ebb::Renderer *renderer = new Ebb::Renderer(root, new Ebb::Window(800,600, "Test window"));
    manager.addManager(renderer);

    manager.run();

    Ebb::Window::teardown();

    return 0;
}