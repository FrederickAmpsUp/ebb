#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ebb/object.hpp>
#include <ebb/render/rendertex.hpp>
#include <ebb/external/glm/glm.hpp>
#include <ebb/external/glm/gtc/matrix_transform.hpp>
#include <ebb/internal/internals.hpp>
#include <new>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Camera : public Object {
public:
    Camera(Ebb::Node *parent, unsigned int pixelWidth = 800, unsigned int pixelHeight = 600, float fov = 60) : Object(parent) {
        this->background = glm::vec3(0.0);
        this->_width = pixelWidth;
        this->_height = pixelHeight;

        this->_tex = new Ebb::RenderTexture(nullptr, this->_width, this->_height);

        this->_proj_matrix = glm::perspective(glm::radians(fov), (float)pixelWidth / (float)pixelHeight, 0.1f, 100.0f);
    }

    Camera(Ebb::Node *parent, unsigned int pixelWidth, unsigned int pixelHeight, float fov, glm::vec3 background) : Object(parent) {
        this->background = background;
        this->_width = pixelWidth;
        this->_height = pixelHeight;

        this->_tex = new Ebb::RenderTexture(nullptr, this->_width, this->_height);

        this->_proj_matrix = glm::perspective(glm::radians(fov), (float)pixelWidth / (float)pixelHeight, 0.1f, 100.0f);
    }

    virtual void update() override {
        this->_tex->clear(this->background);
        Ebb::Internals::activeCamera = this;
        for (Node *node : this->find_all<Ebb::Renderable>()) {
            (dynamic_cast<Ebb::Renderable *>(node))->draw();
        }
        this->Node::update();
    }

    virtual void save(FILE *file) override {
        this->Object::save(file);
        fwrite(&this->background, sizeof(glm::vec3), 1, file);
        fwrite(&this->_width, sizeof(unsigned int), 1, file);
        fwrite(&this->_height, sizeof(unsigned int), 1, file);
        this->_tex->save(file);
        fwrite(&this->_proj_matrix, sizeof(glm::mat4), 1, file);
    }
    virtual void load(FILE *file) override {
        this->Object::load(file);
        fread(&this->background, sizeof(glm::vec3), 1, file);
        fread(&this->_width, sizeof(unsigned int), 1, file);
        fread(&this->_height, sizeof(unsigned int), 1, file);
        this->_tex->load(file);
        fread(&this->_proj_matrix, sizeof(glm::mat4), 1, file);
        float fov = 2.0f * atan(1.0f / this->_proj_matrix[1][1]);
        new (this) Camera(this->get_parent(), this->_width, this->_height, glm::degrees(fov), this->background);
    }

    virtual Node *construct(Node *parent) override {
        return new Camera(parent);
    }

    glm::vec3 background;
private:
    unsigned int _width, _height;
    Ebb::RenderTexture *_tex;

    glm::mat4x4 _proj_matrix;

    getter(_width)
    getter(_height)
    getter(_tex)

    getter(_proj_matrix)
}; // end class Camera
}; // end namespace Ebb

#endif // CAMERA_HPP
#include <ebb/internal/cleanup.hpp>