#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/node.hpp>
#include <ebb/transform.hpp>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Object : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors

    /**
     * @brief Get the world-space tranform of this object.
     * This function must have a chain up to the root to be accurate.
     * For example, this is good:
     * Object
     *  - NotObject
     *  - Object (or inherited from Object)
     *    - Object (can get world-space position)
     * 
     * This is not:
     * Object
     *   - NotObject
     *     - Object (cannot get world-space position as it doesn't have a chain of Objects back to the root)
     *   - Object
    */
    Ebb::Transform world_pos() {
        if (dynamic_cast<Object *>(this->get_parent()) != nullptr) return dynamic_cast<Object *>(this->get_parent())->world_pos() * this->_transform;
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