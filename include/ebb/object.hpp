#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/node.hpp>
#include <ebb/transform.hpp>
#include <stdio.h>
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
        if (dynamic_cast<Object *>(this->get_parent()) != nullptr) return dynamic_cast<Object *>(this->get_parent())->world_pos() * this->transform;
        return this->transform;
    }

    virtual void save(FILE *file) override {
        printf("object save\n");
        this->Node::save(file);
        glm::mat4x4 mat = this->transform.get_transform_matrix(); // save the transformation matrix
        fwrite(&mat, sizeof(mat), 1, file);
    }

    virtual void load(FILE *file) override {
        this->Node::load(file);
        glm::mat4x4 mat;
        fread(&mat, sizeof(mat), 1, file); // load the transformation matrix
        this->transform = { mat };
    }

    Ebb::Transform transform;
private:
}; // end class Object
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>
#endif // OBJECT_HPP