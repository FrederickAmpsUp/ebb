#include <ebb/node.hpp>
#include <ebb/window.hpp>

using Node = Ebb::Node;
int main(int argc, char **argv) {
    Node *root = new Node;
    Node *win = new Ebb::WindowManager(root, new Ebb::Window(800,600, "Test window"));

    Ebb::NodeTreeManager manager(root);
    manager.run();

    Ebb::Window::teardown();

    return 0;
}