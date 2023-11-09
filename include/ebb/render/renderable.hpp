#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <ebb/component.hpp>

namespace Ebb {

class Renderable : public Ebb::Component {
public:
    using Ebb::Component::Component; // inherited constructors
    virtual void draw() {};
private:
}; // end class Renderable
}; // end namespace Ebb

#endif