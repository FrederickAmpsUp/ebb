#include <ebb/node.hpp>
#include <ebb/window.hpp>

int main(int argc, char **argv) {

    FILE *file = fopen("tests/window.ebb", "rb");
    if (file == nullptr) {
        printf("Error opening file.\n");
        return -1;
    }

    Ebb::Node *loader = new Ebb::Node;
    loader->addType<Ebb::Node>("Node");
    loader->addType<Ebb::WindowManager>("WindowManager");

    Ebb::Node *root = loader->loadNode(file);
    root->print();

    Ebb::NodeTreeManager manager(root);
    manager.run();

    return 0;
}