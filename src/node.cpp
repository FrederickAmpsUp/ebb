#include <ebb/node.hpp>
#include <stdint.h>

/**
 * Node memory layout:
 *     |type|      type of the node (c-string) [IMPORTANT] must be found in the node tree's constructor table. 
 *     |nChildren| number of children (4 bytes, unsigned)
 *     |child*|    list of children's data
 *     |other|     any other data, defined per inherited class
*/
void Ebb::Node::save(FILE *file) {
    if (file == NULL) {
            // TODO: show error message
        return;
    }
    char type[] = "Node"; // root name of the class
    std::shared_ptr<Ebb::Node> root = this->findRoot();
        // make sure the root node knows of this type
    root->addType<Ebb::Node>(std::string(type));
        // write the node type to the file
    fwrite(&type, 1, (sizeof(type) / sizeof(char)), file);

        // write the number of children to the file
    uint32_t nChildren = this->children.size();
    fwrite(&nChildren, 1, sizeof(uint32_t), file);

        // write the children to the file
    for (std::shared_ptr<Ebb::Node> child : this->children) {
        child->save(file);
    }

    // no other data for base node class (there should be in inherited classes)
}

void Ebb::Node::load(FILE *file) {
    if (file == NULL) {
            // TODO: show error message
        return;
    }
    std::shared_ptr<Ebb::Node> root = this->findRoot();
    uint32_t nChildren;  // get the number of children from the file
    fread(&nChildren, 1, sizeof(uint32_t), file);

    for (int i = 0; i < nChildren; i++) {
        std::shared_ptr<Ebb::Node> child = root->loadNode(file);
        child->parent = std::make_shared<Ebb::Node>(this);
        this->children.push_back(child);
    }
}

std::shared_ptr<Ebb::Node> Ebb::Node::loadNode(FILE *file) {
    if (file == NULL) {
            // TODO: show error messages
        return;
    }
    std::shared_ptr<Ebb::Node> root = this->findRoot();

    std::string type;
    char c;
    while ((c = fgetc(file)) != '\0') {
        if (c == EOF) {
                // TODO: show error messages
            return;
        }
        type += c;
    }
    std::shared_ptr<Ebb::Node> node = root->construct(type);
}