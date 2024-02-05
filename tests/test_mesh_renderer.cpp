#include <ebb/node.hpp>
#include <ebb/window.hpp>
#include <ebb/renderer.hpp>
#include <ebb/mesh/renderer.hpp>
#include <stdio.h>

using Node = Ebb::Node;
int main(int argc, char **argv) {
    Ebb::Window::setup();
    Node *root = new Node;
    Ebb::NodeTreeManager manager(root);
    Ebb::Renderer *renderer = new Ebb::Renderer(root, new Ebb::Window(800,600, "Test window"));
    manager.addManager(renderer);

    Ebb::MeshRenderer *suzanne = new Ebb::MeshRenderer(root, Ebb::Mesh::loadOBJ("models/monke.obj"));
    
    manager.run();

    Ebb::Window::teardown();

    return 0;
}