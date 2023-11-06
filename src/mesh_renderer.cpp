#include <ebb/render/mesh_renderer.hpp>
#include <ebb/internal/internals.hpp>
#include <ebb/render/camera.hpp>
#include <ebb/error.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Ebb::MeshRenderer::MeshRenderer(Ebb::Node *parent, Ebb::Shader *shader, Ebb::Mesh *mesh) : Ebb::Renderable(parent) {
    // TODO: load the mesh into VAO, VBO and EBO

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    this->_num_tris  = mesh-> get_inds().size() / 3;
    this->_num_verts = mesh->get_verts().size();

    std::vector<glm::vec3> verts = mesh->get_verts();
    std::vector<glm::vec3> norms = mesh->get_norms();
    std::vector<glm::vec2> texts = mesh->get_texts();
    
        /*
    INFO LAYOUT:
        |vX| |vY| |vZ|  |nX| |nY| |nZ|  | tX || tY |
        | vertex pos |  |   normal   |  | texcoord |
        |  3 floats  |  |  3 floats  |  | 2 floats |
        |                  8 floats                |
    */
    float *meshInfo = (float *)malloc(this->_num_verts * 8 * sizeof(float));

    for (unsigned int i = 0; i < this->_num_verts; ++i) {
        meshInfo[i * 8 + 0] = verts[i].x;
        meshInfo[i * 8 + 1] = verts[i].y;
        meshInfo[i * 8 + 2] = verts[i].z;

        meshInfo[i * 8 + 3] = norms[i].x;
        meshInfo[i * 8 + 4] = norms[i].y;
        meshInfo[i * 8 + 5] = norms[i].z;

        meshInfo[i * 8 + 6] = texts[i].x;
        meshInfo[i * 8 + 7] = texts[i].y;
    }

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->_num_verts * 8 * sizeof(float), meshInfo, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_num_tris * 3 * sizeof(unsigned int), &mesh->get_inds()[0], GL_STATIC_DRAW);

    /*
        Index 0 is vertex position, index 1 is normal, index 2 is texcoord.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    this->_shader = shader;
}

void Ebb::MeshRenderer::draw() {
    this->_shader->set_uniform<glm::mat4x4>("objectMatrix", _get_transform_matrix());
    if (Ebb::Internals::activeCamera != nullptr)
        this->_shader->set_uniform<glm::mat4x4>("cameraMatrix", Ebb::Internals::activeCamera->get_proj_matrix() * Ebb::Internals::activeCamera->transform.get_transform_matrix());
    this->_shader->use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3 * this->_num_tris, GL_UNSIGNED_INT, 0);
}

void Ebb::MeshRenderer::setup() {

}
void Ebb::MeshRenderer::update() {

}