#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <ebb/component.hpp>

namespace Ebb {

class Renderable : public Ebb::Component {
public:
    using Ebb::Component::Component; // inherited constructors
    virtual void draw() {};
    virtual Node *construct(Node *parent) override {
        return new Renderable(parent);
    }

    virtual void save(FILE *file) override { this->Component::save(file); }
    virtual void load(FILE *file) override { this->Component::load(file); }
private:
}; // end class Renderable
}; // end namespace Ebb

#endif