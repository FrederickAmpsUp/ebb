#include <ebb/node.hpp>

int main(int argc, char **argv) {

    FILE *file = fopen("tests/node_test.ebb", "rb");
    if (file == nullptr) {
        printf("Error opening file.\n");
        return -1;
    }

    Ebb::Node *loader = new Ebb::Node;
    loader->addType<Ebb::Node>("Node");

    Ebb::Node *root = loader->loadNode(file);
    root->print();

    return 0;
}