#include <ebb/mesh/renderer.hpp>

Ebb::MeshRenderer::MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh) : Ebb::Renderable(parent), mesh(mesh) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->mesh->verts.size()*sizeof(float)*3, &this->mesh->verts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh->indices.size()*sizeof(uint32_t), &this->mesh->indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void *)0); // position [TODO: normals, texcoords]
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
Ebb::MeshRenderer::MeshRenderer(Ebb::Mesh *mesh) : Ebb::MeshRenderer(nullptr, mesh) {}

void Ebb::MeshRenderer::draw() {
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->mesh->indices.size());
}