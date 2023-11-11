#include <ebb/mesh.hpp>
#include <ebb/error.hpp>
#include <stdio.h>

Ebb::Mesh::Mesh(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return;
    }

    char byte;
    bool parsingFaces = false;
    while ((byte = fgetc(file)) != EOF) {
        if (!parsingFaces && (byte == 'f')) {
            parsingFaces = true;
        }
        if (!parsingFaces) {
            if (byte != 'v') {
                fprintf(stderr, "Invalid ebb mesh file \"%s\": expected 'v', got 0x%02x\n", filename, byte);
                exit(2);
            }
            float posX, posY, posZ;
            float texX, texY;
            float normX, normY, normZ;

            size_t status = 0;

            status += fread(&posX, sizeof(posX), 1, file);
            status += fread(&posY, sizeof(posY), 1, file);
            status += fread(&posZ, sizeof(posZ), 1, file);

            status += fread(&texX, sizeof(texX), 1, file);
            status += fread(&texY, sizeof(texY), 1, file);
            
            status += fread(&normX, sizeof(normX), 1, file);
            status += fread(&normY, sizeof(normY), 1, file);
            status += fread(&normZ, sizeof(normZ), 1, file);

            if (status != 8) {
                fprintf(stderr, "Failed reading vertex data from ebb mesh file \"%s\".", filename);
                exit(2);
            }

            this->_verts.push_back(glm::vec3(posX, posY, posZ));
            this->_texts.push_back(glm::vec2(texX, texY));
            this->_norms.push_back(glm::vec3(normX, normY, normZ));
        } else {
            if (byte != 'f') {
                fprintf(stderr, "Invalid ebb mesh file \"%s\": expected 'f', got 0x%02x\n", filename, byte);
                exit(2);
            }
            unsigned int v0, v1, v2;

            size_t status = 0;

            status += fread(&v0, sizeof(v0), 1, file);
            status += fread(&v1, sizeof(v1), 1, file);
            status += fread(&v2, sizeof(v2), 1, file);

            if (status != 3) {
                fprintf(stderr, "Failed reading face data from ebb mesh file \"%s\".", filename);
                exit(2);
            }
            this->_inds.push_back(v0);
            this->_inds.push_back(v1);
            this->_inds.push_back(v2);
        }
    }
}