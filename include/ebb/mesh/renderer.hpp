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

    void draw() override;
private:
    Ebb::Mesh *mesh;
    Ebb::ObjectShader *shader;

    unsigned int VAO, VBO, EBO;
};
};