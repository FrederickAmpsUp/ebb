#include <ebb/render/mesh_renderer.hpp>
#include <ebb/error.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Ebb::MeshRenderer::draw() {
    this->_shader->set_uniform<glm::mat4x4>("objectMatrix", _get_transform_matrix());
    this->_shader->use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3 * this->_num_triangles, GL_UNSIGNED_INT, 0);
}

void Ebb::MeshRenderer::setup() {

}
void Ebb::MeshRenderer::update() {

}