#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <ebb/component.hpp>

namespace Ebb {

class Renderable : public Ebb::Component {
public:
    using Ebb::Component::Component; // inherited constructors
    virtual void draw() {};

    virtual void save(FILE *file) override { this->Node::save(file); printf("renderable save\n"); }
    virtual void load(FILE *file) override { this->Node::load(file); }
private:
}; // end class Renderable
}; // end namespace Ebb

#endif