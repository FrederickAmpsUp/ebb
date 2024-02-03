#pragma once

#include <ebb/node.hpp>
#include <ebb/mesh/mesh.hpp>
#include <ebb/renderer.hpp>

namespace Ebb {
class MeshRenderer : public Renderable {
public:
    MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh);
    MeshRenderer(Ebb::Mesh *mesh);

    void draw() override;
private:
    Ebb::Mesh *mesh;

    unsigned int VAO, VBO, EBO;
};
};