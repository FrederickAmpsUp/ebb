#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <ebb/node.hpp>
#include <stdio.h>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Component : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors

    virtual void save(FILE *file) override { this->Node::save(file); printf("WTF\n"); }
    virtual void load(FILE *file) override { this->Node::load(file); }
private:

}; // end class Component
}; // end namespace Ebb


#include <ebb/internal/cleanup.hpp>

#endif // COMPONENT_HPP