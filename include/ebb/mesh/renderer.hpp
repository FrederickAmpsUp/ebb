#pragma once

#include <ebb/node.hpp>
#include <ebb/mesh/mesh.hpp>
#include <ebb/renderer.hpp>
#include <ebb/shader.hpp>

namespace Ebb {
class MeshRenderer : public Renderable {
public:
    MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh, Ebb::ObjectShader *shader);
    MeshRenderer(Ebb::Mesh *mesh, Ebb::ObjectShader *shader);
    MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh);
    MeshRenderer(Ebb::Mesh *mesh);

    void draw(Ebb::Math::mat4x4 view=Ebb::Math::mat4x4(1.0), Ebb::Math::mat4x4 proj=Ebb::Math::mat4x4(1.0)) override;
private:
    Ebb::Mesh *mesh;
    Ebb::ObjectShader *shader;

    unsigned int VAO, VBO, EBO;
};
};