#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/node/node.hpp>
#include <ebb/transform.hpp>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Object : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors

    Ebb::Transform world_pos() {
        if (static_cast<Object *>(this->get_parent()) != nullptr) return static_cast<Object *>(this->get_parent())->world_pos() * this->_transform;
        return this->_transform;
    }
private:
    Ebb::Transform _transform;
    getter(_transform)
    setter(_transform)
}; // end class Object
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>
#endif // OBJECT_HPP