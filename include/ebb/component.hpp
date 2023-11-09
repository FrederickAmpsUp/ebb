#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <ebb/node.hpp>
#include <stdio.h>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Component : public Ebb::Node {
public:
    using Ebb::Node::Node; // inherited constructors
private:

}; // end class Component
}; // end namespace Ebb


#include <ebb/internal/cleanup.hpp>

#endif // COMPONENT_HPP