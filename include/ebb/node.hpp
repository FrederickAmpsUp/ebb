#pragma once

#include <ebb/serializable.hpp>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <string>

namespace Ebb {
class Node : public Ebb::Serializable {
public:
    /**
     * Constructor. Assumes root node with no children.
    */
    Node() : children(0), parent(nullptr) {}
    /**
     * Constructor. Assumes root node with children.
    */
    Node(std::vector<std::shared_ptr<Ebb::Node>> children) : children(children), parent(nullptr) {}
    /**
     * Constructor. Assumes branch node.
    */
    Node(std::vector<std::shared_ptr<Ebb::Node>> children, std::shared_ptr<Ebb::Node> parent) : children(children), parent(parent) {}
    /**
     * Constructor. Assumes leaf node.
    */
    Node(std::shared_ptr<Ebb::Node> parent) : children(0), parent(parent) {}

    /**
     * Serializable overrides
    */
    virtual void save(FILE *file) override;
    virtual void load(FILE *file) override;
    std::shared_ptr<Ebb::Node> loadNode(FILE *file);

    /**
     * Traverse the node tree to find the tree's root.
     * @retval The root node of the tree.
    */
    std::shared_ptr<Ebb::Node> findRoot() {
            // this is the root
        if (this->parent == nullptr) return std::make_shared<Ebb::Node>(this);
            // parent will find it (recurse up the tree)
        return this->parent->findRoot();
    }

template <typename T>
    void addType(std::string name) {
        this->constructors[name] = []{ std::make_shared<T>() };
            // make sure children have it too (recurse down the tree)
        for (std::shared_ptr<Ebb::Node> child : this->children) {
            child->addType<T>(name);
        }
    }

    std::shared_ptr<Ebb::Node> construct(std::string typeName) {
        if (this->constructors.find(typeName) == this->constructors.end()) {
            if (this->parent != nullptr) return this->parent->construct(typeName);
            // todo: show error message (node type not found)
            return nullptr;
        } else {
            return this->constructors[typeName]();
        }

    }

private:
    std::vector<std::shared_ptr<Ebb::Node>> children;
    std::shared_ptr<Ebb::Node> parent;

    std::map<std::string, std::function<std::shared_ptr<Ebb::Node>()>> constructors;
};
};