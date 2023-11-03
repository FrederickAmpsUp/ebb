#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include <ebb/component.hpp>
#include <ebb/object.hpp>
#include <ebb/render/renderable.hpp>
#include <ebb/internal/classutil.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/external/glm/glm.hpp>

namespace Ebb {

class MeshRenderer : public Ebb::Renderable {
public:
    MeshRenderer(Ebb::Node *parent, Ebb::Shader *shader) : Ebb::Renderable(parent) {
        this->_shader = shader;
    }

    void draw() override;

    void setup()  override;
    void update() override;
private:
    unsigned int VAO, VBO;
    unsigned int EBO;

    unsigned int _num_triangles;
    getter(_num_triangles);

    Ebb::Shader *_shader;
    getter(_shader)
    setter(_shader)

    glm::mat4x4 _get_transform_matrix() {
        Ebb::Node *node = this;

        while (node->get_parent() != nullptr) { // find the first parent with a transform matrix
            if (static_cast<Ebb::Object *>(node->get_parent()) != nullptr) return static_cast<Ebb::Object *>(node->get_parent())->world_pos().get_transform_matrix();
            node = node->get_parent();
        }

        if (static_cast<Ebb::Object *>(node) != nullptr) return static_cast<Ebb::Object *>(node)->world_pos().get_transform_matrix();
        return glm::mat4x4(1.0f);
    }
    
}; // end class MeshRenderer
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>

#endif // MESH_RENDERER_HPP