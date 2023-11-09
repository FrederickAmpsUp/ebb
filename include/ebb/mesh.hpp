#ifndef MESH_HPP
#define MESH_HPP

#include <ebb/external/glm/glm.hpp>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class Mesh {
public:
    Mesh(std::vector<glm::vec3>& vertices,
         std::vector<glm::vec3>& normals,
         std::vector<glm::vec2>& texcoords,
         std::vector<unsigned int>& indices
    ) : _verts(vertices), _norms(normals), _texts(texcoords), _inds(indices) {}

    Mesh(const char *filename);

    void save(FILE *file) {
        glm::vec3 *vertices  = this->_verts.data();
        glm::vec3 *normals   = this->_norms.data();
        glm::vec2 *texcoords = this->_texts.data();

        unsigned int *indices = this->_inds.data();

        int nVerts = this->_verts.size();
        fwrite(&nVerts, sizeof(int), 1, file);  // Number of vertices
        fwrite(vertices, sizeof(glm::vec3) * nVerts, 1, file); // Vertices array

        int nNorms = this->_norms.size();
        fwrite(&nNorms, sizeof(int), 1, file);  // Number of normals
        fwrite(normals, sizeof(glm::vec3) * nNorms, 1, file); // Normals array

        int nTexts = this->_texts.size();
        fwrite(&nTexts, sizeof(int), 1, file);  // Number of texcoords
        fwrite(texcoords, sizeof(glm::vec2) * nTexts, 1, file); // Texcoords array

        int nInds = this->_inds.size();
        fwrite(&nInds, sizeof(int), 1, file); // Number of indices
        fwrite(indices, sizeof(unsigned int) * nInds, 1, file); // Indices array
    }

    void load(FILE *file) {
        int nVerts;
        fread(&nVerts, sizeof(int), 1, file);
        glm::vec3 *vertices = (glm::vec3 *)malloc(nVerts * sizeof(glm::vec3));
        fread(vertices, nVerts * sizeof(glm::vec3), 1, file);
        this->_verts = std::vector<glm::vec3>(vertices, vertices + nVerts * sizeof(glm::vec3));

        int nNorms;
        fread(&nNorms, sizeof(int), 1, file);
        glm::vec3 *normals = (glm::vec3 *)malloc(nNorms * sizeof(glm::vec3));
        fread(normals, nNorms * sizeof(glm::vec3), 1, file);
        this->_norms = std::vector<glm::vec3>(normals, normals + nNorms * sizeof(glm::vec3));

        int nTexts;
        fread(&nTexts, sizeof(int), 1, file);
        glm::vec2 *textures = (glm::vec2 *)malloc(nTexts * sizeof(glm::vec2));
        fread(textures, nTexts * sizeof(glm::vec2), 1, file);
        this->_texts = std::vector<glm::vec2>(textures, textures + nTexts * sizeof(glm::vec2));

        int nInds;
        fread(&nInds, sizeof(int), 1, file);
        unsigned int *indices = (unsigned int *)malloc(nInds * sizeof(unsigned int));
        fread(indices, nInds * sizeof(unsigned int), 1, file);
        this->_inds = std::vector<unsigned int>(indices, indices + nInds * sizeof(unsigned int));
    }
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