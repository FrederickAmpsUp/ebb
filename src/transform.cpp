#include <ebb/transform.hpp>

Ebb::Math::mat4x4 Ebb::Transform::totalTransform() {
    Ebb::Math::mat4x4 result = Ebb::Math::mat4x4(1.0);
    Ebb::Node *node = this;

    while (node != nullptr) {
        if (dynamic_cast<Ebb::Transform *>(node) != nullptr) result = result * dynamic_cast<Ebb::Transform *>(node)->matrix();
        node = node->getParent();
    }

    return result;
}