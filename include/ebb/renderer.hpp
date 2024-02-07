#pragma once

#include <ebb/node.hpp>
#include <ebb/window.hpp>
#include <ebb/math.hpp>
#include <vector>

namespace Ebb {
class Renderable : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors
    virtual void draw(Ebb::Math::mat4x4 view=Ebb::Math::mat4x4(1.0), Ebb::Math::mat4x4 proj=Ebb::Math::mat4x4(1.0)) = 0;
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