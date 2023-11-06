#ifndef INTERNALS_HPP
#define INTERNALS_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/object.hpp>

namespace Ebb {
class Camera; // circular include fix
namespace Internals {

Ebb::Camera *activeCamera;

}; // end namespace Internals
}; // end namespace Ebb

#endif