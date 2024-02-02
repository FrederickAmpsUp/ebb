#pragma once

#include <ebb/serializable.hpp>
#include <stdio.h>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <type_traits>

namespace Ebb {
class Node : public Ebb::Serializable {
public:
    /**
     * Constructor. Assumes root node with no children.
    */
    Node() : parent(nullptr) {
        active = true;
    }
    /**
     * Constructor. Assumes root node with children.
    */
    Node(std::vector<Ebb::Node *> children) : children(children), parent(nullptr) {
        active = true;
        for (Ebb::Node *child : children) {
            child->parent = this;
    }
    }
    /**
     * Constructor. Assumes branch node.
    */
    Node(std::vector<Ebb::Node *> children, Ebb::Node *parent) : children(children), parent(parent) {
        active = true;
        for (Ebb::Node *child : children) {
            child->parent = this;
        }

        parent->addChild(this);
    }
    /**
     * Constructor. Assumes leaf node.
    */
    Node(Ebb::Node *parent) : parent(parent) {
        active = true;
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

    /**
     * Setup the node. This may be overridden, but at the end of the override, the line `this->Ebb::Node::setup();` must be present in order to update children.
    */
    virtual void setup() {
            // recurse down the tree.
        for (Ebb::Node *child : this->children) {
            child->setup();
        }
    }
    /**
     * Run the node's loop (called every frame, after node manager updates). This may be overridden, but at the end of the override, the line `this->Ebb::Node::update();` must be present in order to update children.
    */
    virtual void update() {
                    // recurse down the tree
        for (Ebb::Node *child : this->children) {
            child->update();
        }
    }

template <typename T, typename = std::enable_if_t<std::is_base_of_v<Ebb::Node, T>>>
    std::vector<T *> findAll() {
        std::vector<T *> result;
        for (Ebb::Node *child : this->children) {
            if (dynamic_cast<T *>(child) != nullptr) result.push_back(dynamic_cast<T *>(child));
            std::vector<T *> chRes = child->findAll<T>();
            result.insert(result.end(), chRes.begin(), chRes.end());
        }

        return result;
    }

    bool active;
    virtual char *typeName() { return (char *)"Node"; }
private:
    std::vector<Ebb::Node *> children;
    Ebb::Node *parent;

    std::map<std::string, std::function<Ebb::Node *()>> constructors;
};

/**
 * Abstract class for things that do stuff to nodes. An example is Ebb::Renderer.
*/
class NodeManager {
public:
    NodeManager() {};
    virtual void  setup() = 0;
    virtual void update() = 0;
};

class NodeTreeManager {
public:
    NodeTreeManager(Ebb::Node *tree) : tree(tree), managers({}) {}
    NodeTreeManager(Ebb::Node *tree, const std::vector<Ebb::NodeManager *>& managers) : tree(tree), managers(managers) {}

    void addManager(Ebb::NodeManager *manager) { this->managers.push_back(manager); }

    void run() {
        if (this->tree == nullptr) return;

        for (Ebb::NodeManager *manager : this->managers) {
            manager->setup();
        }
        this->tree->setup();

        for (; this->tree->active;) {
            for (Ebb::NodeManager *manager : this->managers) {
                manager->update();
            }
            this->tree->update();
        }
    }
private:
    Ebb::Node *tree;
    std::vector<Ebb::NodeManager *> managers;
};
};