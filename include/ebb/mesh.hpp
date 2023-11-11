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

        size_t nVerts = this->_verts.size();
        fwrite(&nVerts, sizeof(size_t), 1, file);  // Number of vertices
        printf("nVerts: %zu\n", nVerts);
        fwrite(vertices, sizeof(glm::vec3) * nVerts, 1, file); // Vertices array

        size_t nNorms = this->_norms.size();
        fwrite(&nNorms, sizeof(size_t), 1, file);  // Number of normals
        printf("nNorms: %zu\n", nNorms);
        fwrite(normals, sizeof(glm::vec3) * nNorms, 1, file); // Normals array

        size_t nTexts = this->_texts.size();
        fwrite(&nTexts, sizeof(size_t), 1, file);  // Number of texcoords
        printf("nTexts: %zu\n", nTexts);
        fwrite(texcoords, sizeof(glm::vec2) * nTexts, 1, file); // Texcoords array

        size_t nInds = this->_inds.size();
        fwrite(&nInds, sizeof(size_t), 1, file); // Number of indices
        printf("nInds: %zu\n", nInds);
        fwrite(indices, sizeof(unsigned int) * nInds, 1, file); // Indices array
    }

    void load(FILE *file) {
        size_t nVerts;
        fread(&nVerts, sizeof(size_t), 1, file);
        printf("nVerts: %zu\n", nVerts);
        glm::vec3 *vertices = (glm::vec3 *)malloc(nVerts * sizeof(glm::vec3));
        fread(vertices, nVerts * sizeof(glm::vec3), 1, file);
        this->_verts = std::vector<glm::vec3>(vertices, vertices + (nVerts-1));

        size_t nNorms;
        fread(&nNorms, sizeof(size_t), 1, file);
        printf("nNorms: %zu\n", nNorms);
        glm::vec3 *normals = (glm::vec3 *)malloc(nNorms * sizeof(glm::vec3));
        fread(normals, nNorms * sizeof(glm::vec3), 1, file);
        this->_norms = std::vector<glm::vec3>(normals, normals + (nNorms-1));

        size_t nTexts;
        fread(&nTexts, sizeof(size_t), 1, file);
        printf("nTexts: %zu\n", nTexts);
        glm::vec2 *textures = (glm::vec2 *)malloc(nTexts * sizeof(glm::vec2));
        fread(textures, nTexts * sizeof(glm::vec2), 1, file);
        this->_texts = std::vector<glm::vec2>(textures, textures + (nTexts-1));

        size_t nInds;
        fread(&nInds, sizeof(size_t), 1, file);
        printf("nInds: %zu\n", nInds);
        unsigned int *indices = (unsigned int *)malloc(nInds * sizeof(unsigned int));
        fread(indices, nInds * sizeof(unsigned int), 1, file);
        this->_inds = std::vector<unsigned int>(indices, indices + (nInds-1));
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