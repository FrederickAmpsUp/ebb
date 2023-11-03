#ifndef MESH_HPP
#define MESH_HPP

#include <ebb/external/glm/glm.hpp>
#include <ebb/internal/classutil.hpp>
#include <vector>

namespace Ebb {

class Mesh {
public:
    Mesh(std::vector<glm::vec3>& vertices,
         std::vector<glm::vec3>& normals,
         std::vector<glm::vec2>& texcoords,
         std::vector<unsigned int>& indices
    ) : _verts(vertices), _norms(normals), _texts(texcoords), _inds(indices) {}

    Mesh(const char *filename);
private:
    std::vector<glm::vec3> _verts;
    std::vector<glm::vec3> _norms;
    std::vector<glm::vec2> _texts;

    std::vector<unsigned int> _inds;

    getter(_verts)
    getter(_norms)
    getter(_texts)
    getter(_inds)
}; // end class Mesh
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>

#endif // MESH_HPP