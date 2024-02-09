#include <ebb/mesh/renderer.hpp>
#include <ebb/transform.hpp>
#include <ebb/math.hpp>
#include <GL/glut.h>

Ebb::MeshRenderer::MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh, Ebb::ObjectShader *shader) : Ebb::Renderable(parent), mesh(mesh) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    size_t nVerts = this->mesh->verts.size();
    std::vector<float> interlaced;
    interlaced.reserve(nVerts * (3 + 3 + 2)); // 3 for position, 3 for normal, 2 for texcoord

    for (size_t i = 0; i < nVerts; ++i) {
        interlaced.push_back(this->mesh->verts[i][0]);
        interlaced.push_back(this->mesh->verts[i][1]);
        interlaced.push_back(this->mesh->verts[i][2]);

        interlaced.push_back(this->mesh->normals[i][0]);
        interlaced.push_back(this->mesh->normals[i][1]);
        interlaced.push_back(this->mesh->normals[i][2]);

        interlaced.push_back(this->mesh->texcoords[i][0]);
        interlaced.push_back(this->mesh->texcoords[i][1]);
    }

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(float) * (3 + 3 + 2), interlaced.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh->indices.size()*sizeof(uint32_t), &this->mesh->indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *)0); // position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *)(3 * sizeof(float))); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *)(6 * sizeof(float))); // texcoord
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (shader) this->shader = shader;
    else this->shader = new Ebb::ObjectShader();
}
Ebb::MeshRenderer::MeshRenderer(Ebb::Node *parent, Ebb::Mesh *mesh) : Ebb::MeshRenderer(parent, mesh, new Ebb::ObjectShader()) {}
Ebb::MeshRenderer::MeshRenderer(Ebb::Mesh *mesh, Ebb::ObjectShader *shader) : Ebb::MeshRenderer(nullptr, mesh, shader) {}
Ebb::MeshRenderer::MeshRenderer(Ebb::Mesh *mesh) : Ebb::MeshRenderer(nullptr, mesh, new Ebb::ObjectShader()) {}

void Ebb::MeshRenderer::draw(Ebb::Math::mat4x4 view, Ebb::Math::mat4x4 proj) {
    Ebb::Math::mat4x4 object = Ebb::Math::mat4x4(1.0);
    if (dynamic_cast<Ebb::Transform *>(this->_parent)) {
        object = dynamic_cast<Ebb::Transform *>(this->_parent)->totalTransform();
    } else {
        std::vector<Ebb::Transform *> transforms = this->_parent->findAll<Ebb::Transform>();
        if (transforms.size() > 0) object = transforms[0]->totalTransform();
    }

    glBindVertexArray(this->VAO);

    glEnable(GL_DEPTH_TEST); // enable z-buffering
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE); // backface culling
    glCullFace(GL_BACK);
    
    this->shader->use();
    this->shader->uniform((char *)"model", object);
    this->shader->uniform((char *)"view", view);
    this->shader->uniform((char *)"projection", proj);
    glDrawElements(GL_TRIANGLES, this->mesh->indices.size(), GL_UNSIGNED_INT, 0);
}