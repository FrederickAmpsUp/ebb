#include <ebb/renderer.hpp>

void Ebb::Renderer::setup() {

}

void Ebb::Renderer::update() {
    this->win->clear();
    std::vector<Ebb::Renderable *> renderables = this->tree->findAll<Ebb::Renderable>();
    for (Ebb::Renderable *renderable : renderables) {
        renderable->draw();
    }

    this->win->update();
}