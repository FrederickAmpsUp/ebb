#include <ebb/mesh/mesh.hpp>

int main(int argc, char **argv) {
    Ebb::Mesh *mesh = Ebb::Mesh::loadOBJ("models/monke.obj");
    printf("%d\n", mesh->verts.size());
    return 0;
}