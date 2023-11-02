#include <ebb/node/node.hpp>
#include <stdio.h>

class SomeNode : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors
    void setup() override {
        printf("SomeNode::setup()\n");
        this->Node::setup();  // setup children
    }

    void update() override {
        printf("SomeNode::update()\n");
        this->Node::update(); // update children
    }
};

int main(int argc, char **argv) {
    SomeNode *rootNode = new SomeNode(nullptr);
    SomeNode *child1 = new SomeNode(rootNode);
    SomeNode *child2 = new SomeNode(rootNode);
    SomeNode *grandchild = new SomeNode(child1);

    rootNode->setup();
    return 0;
}