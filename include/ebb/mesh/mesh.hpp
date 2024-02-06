#pragma once

#include <ebb/node.hpp>
#include <ebb/renderer.hpp>
#include <ebb/math.hpp>
#include <vector>
#include <array>
#include <stdint.h>
#include <string.h>
#include <cmath>

namespace Ebb {
class Mesh {
public:
    Mesh() : verts(), indices(), normals(), texcoords() {}

    static Mesh *loadOBJ(const char* filename) {
        Mesh *mesh = new Mesh;

        FILE* file = fopen(filename, "r");
        if (!file) {
            printf("Failed to open mesh file %s\n", filename);
            return mesh;
        }

        std::vector<std::array<float, 3>> normals;
        std::vector<std::array<float, 2>> texcoords;

        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char type[3];
            sscanf(line, "%2s", type);

            if (strcmp(type, "v") == 0) {
                float x, y, z;
                sscanf(line + 2, "%f %f %f", &x, &y, &z);
                mesh->verts.push_back({x, y, z});
            } else if (strcmp(type, "vt") == 0) {
                float u, v;
                sscanf(line + 3, "%f %f", &u, &v);
                texcoords.push_back({u, v});
            } else if (strcmp(type, "vn") == 0) {
                float nx, ny, nz;
                sscanf(line + 3, "%f %f %f", &nx, &ny, &nz);
                normals.push_back({nx, ny, nz});
            } else if (strcmp(type, "f") == 0) {
                uint32_t v1, v2, v3;
                uint32_t t1, t2, t3;
                uint32_t n1, n2, n3;
                sscanf(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
                mesh->indices.push_back(v1 - 1); // OBJ indices are 1-based
                mesh->indices.push_back(v2 - 1);
                mesh->indices.push_back(v3 - 1);

                mesh->normals.reserve(v1+1);
                mesh->normals.reserve(v2+1);
                mesh->normals.reserve(v3+1);

                mesh->texcoords.reserve(v1+1);
                mesh->texcoords.reserve(v2+1);
                mesh->texcoords.reserve(v3+1);

                mesh->normals[v1-1] = normals[n1-1];
                mesh->normals[v2-1] = normals[n2-1];
                mesh->normals[v3-1] = normals[n3-1];

                mesh->texcoords[v1-1] = texcoords[t1-1];
                mesh->texcoords[v2-1] = texcoords[t2-1];
                mesh->texcoords[v3-1] = texcoords[t3-1];
            }
        }
        fclose(file);

        return mesh;
    }

    std::vector<std::array<float, 3>> verts;
    std::vector<uint32_t> indices;
    std::vector<std::array<float, 3>> normals;
    std::vector<std::array<float, 2>> texcoords;
private:
};
};