#include <ebb/render/shader.hpp>

static const char * const _world_space_vert_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 objectMatrix;

mat3 getNormalMatrix(mat4 modelMatrix) {
    return mat3(transpose(inverse(modelMatrix)));
}

vec3 transformNormal(vec3 normal, mat4 modelMatrix) {
    mat3 normalMatrix = getNormalMatrix(modelMatrix);
    return normalize(normalMatrix * normal);
}

varying vec3 normal;
varying vec3 position;
void main() {
    gl_Position = vec4(aPos, 1.0) * objectMatrix;
    normal = transformNormal(aNorm, objectMatrix);
    position = (vec4(aPos, 1.0) * objectMatrix).xyz;
}
)";

// TODO: camera / projection matrices
namespace Ebb {

class WorldSpaceVertex : public VertexShader {
public:
    WorldSpaceVertex() : VertexShader(_world_space_vert_src) {}
private:

};
};