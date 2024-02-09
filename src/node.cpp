#include <ebb/node.hpp>
#include <ebb/util/file.hpp>
#include <stdint.h>
#include <typeinfo>

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
    char *type = this->typeName(); // root name of the class
        // make sure the root node knows of this type
    Ebb::Node *root = this->findRoot();
    root->addType<Ebb::Node>(std::string(type));
        // write the node type to the file
    Ebb::Util::Files::writeNts(file, type);

        // write the number of children to the file
    uint32_t nChildren = this->children.size();
    fwrite(&nChildren, 1, sizeof(uint32_t), file);

    printf("%d children\n", nChildren);
        // write the children to the file
    for (Ebb::Node *child : this->children) {
        child->save(file);
    }
    printf("okk\n");

    // no other data for base node class (there should be in inherited classes)
}

void Ebb::Node::load(FILE *file) {
    if (file == NULL) {
            // TODO: show error message
        return;
    }
    Ebb::Node *root = this->findRoot();
    uint32_t nChildren;  // get the number of children from the file
    fread(&nChildren, 1, sizeof(uint32_t), file);

    for (int i = 0; i < nChildren; i++) {
        Ebb::Node *child = root->loadNode(file);
        this->addChild(child);
    }

    // other nodes can load data after this
}

Ebb::Node *Ebb::Node::loadNode(FILE *file) {
    if (file == NULL) {
            // TODO: show error messages
        return nullptr;
    }
    Ebb::Node *root = this->findRoot();

    std::string type;
    char c;
    while ((c = fgetc(file)) != '\0') {
        if (c == EOF) {
                // TODO: show error messages
            return nullptr;
        }
        type += c;
    }
    Ebb::Node *node = root->construct(type);
    for (std::pair<std::string, std::function<Ebb::Node *()>> constr : this->constructors) {
        node->addConstructor(std::get<0>(constr), std::get<1>(constr));
    }
    node->load(file);
    return node;
}