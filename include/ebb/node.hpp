#pragma once

#include <ebb/serializable.hpp>
#include <stdio.h>
#include <vector>
#include <map>
#include <functional>
#include <string>

namespace Ebb {
class Node : public Ebb::Serializable {
public:
    /**
     * Constructor. Assumes root node with no children.
    */
    Node() : parent(nullptr) {}
    /**
     * Constructor. Assumes root node with children.
    */
    Node(std::vector<Ebb::Node *> children) : children(children), parent(nullptr) {
        for (Ebb::Node *child : children) {
            child->parent = this;
    }
    }
    /**
     * Constructor. Assumes branch node.
    */
    Node(std::vector<Ebb::Node *> children, Ebb::Node *parent) : children(children), parent(parent) {
        for (Ebb::Node *child : children) {
            child->parent = this;
        }

        parent->addChild(this);
    }
    /**
     * Constructor. Assumes leaf node.
    */
    Node(Ebb::Node *parent) : parent(parent) {
        parent->addChild(this);
    }

    /**
     * Add a child to this node.
     * @param node The child to add
     * @retval void
    */
    void addChild(Ebb::Node *node) {
        node->parent = this;
        this->children.push_back(node);
    }

    /**
     * Serializable overrides
    */
    virtual void save(FILE *file) override;
    virtual void load(FILE *file) override;

    /**
     * Load a node from a file
     * @param file
    */
    Ebb::Node *loadNode(FILE *file);

    /**
     * Traverse the node tree to find the tree's root.
     * @retval The root node of the tree.
    */
    Ebb::Node *findRoot() {
            // this is the root
        if (this->parent == nullptr) return this;
            // parent will find it (recurse up the tree)
        return this->parent->findRoot();
    }

template <typename T>
    void addType(std::string name) {
        this->constructors[name] = std::function([]() -> Ebb::Node* { return new T(); });
            // make sure children have it too (recurse down the tree)
        for (Ebb::Node *child : this->children) {
            child->addType<T>(name);
        }
    }

    Ebb::Node *construct(std::string typeName) {
        if (this->constructors.find(typeName) == this->constructors.end()) {
            if (this->parent != nullptr) return this->parent->construct(typeName);
            // todo: show error message (node type not found)
            return nullptr;
        } else {
            return this->constructors[typeName]();
        }

    }

    virtual void print() {
        printf("Ebb::Node(");
        for (Ebb::Node *child : this->children) {
            child->print();
            printf(", ");
        }
        printf(")");
    }

    void addConstructor(std::string typeName, std::function<Ebb::Node *()> constr) {
        this->constructors[typeName] = constr;
    }
private:
    std::vector<Ebb::Node *> children;
    Ebb::Node *parent;

    std::map<std::string, std::function<Ebb::Node *()>> constructors;
};
};