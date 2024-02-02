#include <ebb/node.hpp>
#include <memory>
#include <stdio.h>

int main(int argc, char **argv) {
    std::shared_ptr<Ebb::Node> root   = std::make_shared<Ebb::Node>();
    std::shared_ptr<Ebb::Node> child1 = std::make_shared<Ebb::Node>(root.get());
    std::shared_ptr<Ebb::Node> child2 = std::make_shared<Ebb::Node>(root.get());

    FILE *file = fopen("tests/node_test.ebb", "wb");
    if (file == nullptr) {
        printf("Error opening file.\n");
        return -1;
    }
    root->save(file);

    return 0;
}