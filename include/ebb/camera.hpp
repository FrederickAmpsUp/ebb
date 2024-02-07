#pragma once

#include <ebb/node.hpp>
#include <ebb/texture.hpp>
#include <ebb/transform.hpp>
#include <ebb/math.hpp>

namespace Ebb {
class Camera : public Ebb::Node {
public:
    using Ebb::Node::Node;
    Camera(Ebb::Node *parent, float fov, int width, int height, float nearclip=0.01f, float farclip=1000.0f) : Node(parent) {
        this->fov = fov;
        this->aspect = (float)width / (float)height;
        this->tex = new Ebb::Texture(width, height);
        this->nearclip = nearclip;
        this->farclip = farclip;
        this->proj = Ebb::Math::perspective(this->fov, this->aspect, nearclip, farclip);
    }

    void update() {
        this->tex->clear();
        Ebb::Node *root = this->findRoot();
        std::vector<Ebb::Renderable *> renderables = root->findAll<Ebb::Renderable>();
        this->tex->bind();

        Ebb::Math::mat4x4 view = Ebb::Math::mat4x4(1.0);
        if (dynamic_cast<Ebb::Transform *>(this->parent)) {
            view = dynamic_cast<Ebb::Transform *>(this->parent)->totalTransform(); 
        } else {
            std::vector<Ebb::Transform *> transforms = this->parent->findAll<Ebb::Transform>();
            if (transforms.size() > 0) view = transforms[0]->totalTransform();
        }

        for (Ebb::Renderable *renderable : renderables) {
            renderable->draw(view, this->proj);
        }
    }

    Ebb::Texture *texture() { return this->tex; }
private:
    float fov = 45.0f;
    float aspect = 1.0f;

    float nearclip = 0.01f, farclip = 1000.0f;
    Ebb::Math::mat4x4 proj = Ebb::Math::perspective(45.0f, 1.0f, 0.01f, 1000.0f);

    Ebb::Texture *tex;
};
};