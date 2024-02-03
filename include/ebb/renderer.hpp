#pragma once

#include <ebb/node.hpp>
#include <ebb/window.hpp>
#include <vector>

namespace Ebb {
class Renderable : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors
    virtual void draw() = 0;
};

class Renderer : public Ebb::NodeManager {
public:
    using Ebb::NodeManager::NodeManager; // inherited constructors
    Renderer(Ebb::Node *tree, Ebb::Window *win) : win(win), tree(tree) {}

    void setup() override;
    void update() override;
private:
    Ebb::Window *win;
    Ebb::Node *tree;
};
};