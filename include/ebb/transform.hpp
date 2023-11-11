#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/external/glm/gtc/matrix_transform.hpp>
#include <ebb/external/glm/gtx/euler_angles.hpp>
#include <ebb/external/glm/gtx/quaternion.hpp>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Transform {
public:
    Transform() {
        this->_transform_matrix = glm::mat4x4(1.0);
    }

    Transform(const glm::vec3& position) {
        this->_transform_matrix = glm::translate(glm::mat4x4(1.0), position);
    }

    Transform(const glm::vec3& position, const glm::vec3& euler) {
        glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0.0f, 0.0f, 1.0f));

        this->_transform_matrix = glm::translate(yawMatrix * pitchMatrix * rollMatrix, position);
    }

    Transform(const glm::vec3& position, const glm::quat& quaternion) {
        this->_transform_matrix = glm::translate(glm::toMat4(quaternion), position);
    }

    Transform(const glm::vec3& position, const glm::vec3& euler, const glm::vec3& scale) {
        glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0.0f, 0.0f, 1.0f));

        this->_transform_matrix = glm::translate(glm::scale(glm::mat4(1.0), scale) * yawMatrix * pitchMatrix * rollMatrix, position);
    }

    Transform(const glm::vec3& position, const glm::quat& quaternion, const glm::vec3& scale) {
        this->_transform_matrix = glm::translate(glm::scale(glm::mat4(1.0), scale) * glm::toMat4(quaternion), position);
    }

    Transform(glm::mat4x4 mat) {
        this->_transform_matrix = mat;
    }

    Transform operator*(Transform other) {
        return Transform(this->_transform_matrix * other.get_transform_matrix());
    }

    glm::vec3 position() {
        return glm::vec3(this->_transform_matrix[3]);
    }

    glm::vec3 scale() {
        glm::vec3 size;
        size.x = glm::length(glm::vec3(_transform_matrix[0]));
        size.y = glm::length(glm::vec3(_transform_matrix[1]));
        size.z = glm::length(glm::vec3(_transform_matrix[2]));

        return size;
    }

    void translate(glm::vec3 pos) {
        this->_transform_matrix = glm::translate(this->_transform_matrix, pos);
    }
    void rotate(glm::vec3 euler) {
        this->_transform_matrix *= glm::eulerAngleXYZ(glm::radians(euler.z), glm::radians(euler.y), glm::radians(euler.z));
    }
    void rotate(glm::quat quaternion) {
        this->_transform_matrix *= glm::mat4x4(quaternion);
    }
    void scale(glm::vec3 scale) {
        this->_transform_matrix = glm::scale(this->_transform_matrix, scale);
    }

    void translation(glm::vec3 pos) {
        this->_transform_matrix[3] = glm::vec4(pos, 1.0);
    }
    void rotation(glm::vec3 euler) {
        this->_transform_matrix = glm::translate(glm::mat4(1.0f), this->position()) *
                     (glm::mat4(1.0f) * glm::eulerAngleXYZ(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z))) *
                     glm::scale(glm::mat4(1.0f), this->scale());
    }
private:
    glm::mat4x4 _transform_matrix;
    getter(_transform_matrix);
    setter(_transform_matrix)
}; // end class Transform
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>

#endif // TRANSFORM_HPP