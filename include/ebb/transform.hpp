#pragma once

#include <ebb/math.hpp>
#include <ebb/node.hpp>

namespace Ebb {
class Transform : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors
    Transform(Ebb::Math::mat4x4 transform) : objectMatrix(transform) {}
    Transform(Ebb::Node *parent, Ebb::Math::mat4x4 transform) : objectMatrix(transform), Ebb::Node(parent) {}

    Ebb::Math::mat4x4 totalTransform();
    Ebb::Math::mat4x4 matrix() { return this->objectMatrix; }

    void translate(Ebb::Math::vec3 delta) { this->objectMatrix = Ebb::Math::translate(this->objectMatrix, delta); }
    void rotateY(float delta) { this->objectMatrix = Ebb::Math::rotate(this->objectMatrix, delta, Ebb::Math::vec3(0.0, 1.0, 0.0)); }
private:
    Ebb::Math::mat4x4 objectMatrix = Ebb::Math::mat4x4(1.0);
};
};