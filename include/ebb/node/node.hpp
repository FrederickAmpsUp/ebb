#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <bits/stdc++.h>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

/**
 * @brief Node class.
 * This class should be inherited rather than used directly. It can store objects, cameras, or anything else.
*/
class Node {
public:
    /**
     * @brief Constructor
     * @param parent The parent node. If this is NULL, the node is at the root of the tree.
    */
    Node(Node *parent) : _parent(parent) {
        if (parent != nullptr) parent->add_child(this);
    }

    /**
     * @brief Constructor
     * @param parent The parent node. If this is NULL, the node is at the root of the tree.
     * @param children A vector of child nodes.
    */
    Node(Node *parent, std::vector<Node *>& children) : _parent(parent), _children(children) {
        if (parent != nullptr) parent->add_child(this);
        for (Node *child : _children) {
            Node *child_parent = child->get_parent();
            if (child_parent != nullptr) child_parent->remove_child(child);
            child->set_parent(this);
        }
    }

    /**
     * @brief Returns the idx'th child of the node. This is equivalent to operator[].
     * @param idx The index of the child to return.
    */
    Node *get_child(int idx) {
        return this->_children[idx];
    }

    /**
     * @brief Returns the idx'th child of the node.
    */
    Node *operator[](int idx) {
        return this->_children[idx];
    }

    /**
     * @brief Returns the idx'th occurrence of a child with type T
     * @param idx The index of the child to return.
    */
template <typename T>
    Node *get_child(int idx = 0) {
        int found_idx = 0;
        for (Node *child : this->_children) {
            if (static_cast<T *>(child) != NULL) {
                if (found_idx == idx) return child;
                ++found_idx;
            }
        }
    }

    /**
     * @brief Returns the idx'th occurrence of a sibling (child of parent) with type T
    */
template<typename T>
    Node *get_sibling(int idx = 0) {
        if (this->_parent == NULL) return NULL;
        return this->_parent->get_child<T>(idx);
    }

    /**
     * @brief Add a child to the node
     * @param child The child to be added
    */
    void add_child(Node *child) {
        this->_children.push_back(child);
    }

    /**
     * @brief Remove a child from the node by value
     * @param child The child to be removed
    */
    void remove_child(Node *child) {
        auto it = std::find(_children.begin(), _children.end(),
                    child);
    
        if (it != _children.end()) {
            _children.erase(it);
        }
    }

    /**
     * @brief Setup the node. This recursively calls setup() on all children of the node.
     * This may be overloaded, however, at some point during the overloaded routine, this->Node::setup() must be called to setup children.
     * This function is called once when a new node tree is loaded.
    */
    virtual void setup() {
        for (Node *child : this->_children) {
            child->setup();
        }
    }

    /**
     * @brief Update the node. This recursively updates all children of the node.
     * This may be overloaded, however, at some point during the overloaded routine, this->Node::update() must be called to update children.
     * This function is called once per frame while the engine is running, and the node is in the active node tree.
    */
    virtual void update() {
        for (Node *child : this->_children) {
            child->update();
        }
    }

private:
    std::vector<Node *> _children;
    Node *_parent;

        // Getters
    getter(_children)
    getter(_parent)
    setter(_parent)
}; // end class Node
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>
#endif  // NODE_HPP