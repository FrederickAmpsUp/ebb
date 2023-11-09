#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include <ebb/component.hpp>
#include <ebb/object.hpp>
#include <ebb/render/renderable.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/external/glm/glm.hpp>
#include <ebb/mesh.hpp>
#include <stdio.h>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class MeshRenderer : public Ebb::Renderable {
public:
    MeshRenderer(Ebb::Node *parent, Ebb::Shader *shader = nullptr, Ebb::Mesh *mesh = nullptr);

    void draw() override;

    void setup()  override;
    void update() override;

    virtual void save(FILE *file) override {
        printf("Mesh renderer save\n");
        this->Node::save(file);
        this->_mesh->save(file);
        this->_shader->save(file);

    }
private:
    Ebb::Mesh *_mesh;
    getter(_mesh);

    unsigned int VAO, VBO;
    unsigned int EBO;

    unsigned int _num_tris;
    getter(_num_tris)

    unsigned int _num_verts;
    getter(_num_verts)

    Ebb::Shader *_shader;
    getter(_shader)
    setter(_shader)

    glm::mat4x4 _get_transform_matrix() {
        Ebb::Node *node = this;

        while (node->get_parent() != nullptr) { // find the first parent with a transform matrix
            if (dynamic_cast<Ebb::Object *>(node->get_parent()) != nullptr) return dynamic_cast<Ebb::Object *>(node->get_parent())->world_pos().get_transform_matrix();
            node = node->get_parent();
        }
        if (dynamic_cast<Ebb::Object *>(node) != nullptr)return dynamic_cast<Ebb::Object *>(node)->world_pos().get_transform_matrix();
        return glm::mat4x4(1.0f);
    }
    
}; // end class MeshRenderer
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>

#endif // MESH_RENDERER_HPP