#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ebb/object.hpp>
#include <ebb/render/rendertex.hpp>
#include <ebb/external/glm/glm.hpp>
#include <ebb/external/glm/gtc/matrix_transform.hpp>
#include <ebb/internal/internals.hpp>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Camera : public Object {
public:
    Camera(Ebb::Node *parent, unsigned int pixelWidth, unsigned int pixelHeight, float fov) : Object(parent) {
        this->_width = pixelWidth;
        this->_height = pixelHeight;

        this->_tex = new Ebb::RenderTexture(nullptr, this->_width, this->_height);

        this->_proj_matrix = glm::perspective(glm::radians(fov), (float)pixelWidth / (float)pixelHeight, 0.1f, 100.0f);
    }

    void update() {
        this->_tex->clear();
        Ebb::Internals::activeCamera = this;
        for (Node *node : this->find_all<Ebb::Renderable>()) {
            (dynamic_cast<Ebb::Renderable *>(node))->draw();
        }
        this->Node::update();
    }
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