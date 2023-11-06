#ifndef HALF_LAMBERT_HPP
#define HALF_LAMBERT_HPP

#include <ebb/render/shader.hpp>
#include <ebb/external/glm/glm.hpp>

static const char * const _hlamb_vsh_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 objectMatrix;
uniform mat4 cameraMatrix;

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
    gl_Position = (cameraMatrix * (vec4(aPos, 1.0) * objectMatrix));
    normal = transformNormal(aNorm, objectMatrix);
    position = (cameraMatrix * (vec4(aPos, 1.0) * objectMatrix)).xyz;
}
)";

static const char * const _hlamb_fsh_src = R"(
#version 330 core

uniform vec3 diffuseCol;

uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 ambient;

varying vec3 normal;
varying vec3 position;

layout (location = 0) out vec3 color;

void main() {
    vec3 diffuse = pow(dot(normal, normalize(lightPos - position)) * 0.5 + 0.5, 2.0) * diffuseCol * lightCol;
    color = ambient + diffuse;
}
)";

namespace Ebb {

namespace Shaders {

/**
 * @brief Half-lambertian lighting model from Valve's Half-Life (Source engine)
 * This is a NON-PB lighting model.
 * See https://developer.valvesoftware.com/wiki/Half_Lambert for more information.
 * 
 * Overview:
 *  diffuse = (dot(norm, dirToLight) * .5 + .5)^2 * diffuseCol * lightCol
 *  color = ambient + diffuse
*/
class HalfLambert : public Ebb::Shader {
public:
    HalfLambert(
        glm::vec3 lightPos, glm::vec3 lightCol, glm::vec3 ambient,
        glm::vec3 diffuseCol
    ) : Ebb::Shader(_hlamb_vsh_src, _hlamb_fsh_src) {
        this->lightPos = lightPos;
        this->lightCol = lightCol;
        this->ambient = ambient;
        this->diffuseCol = diffuseCol;
    }

    void use() override {
        this->Ebb::Shader::use();

        this->set_uniform<glm::vec3>("lightPos", this->lightPos);
        this->set_uniform<glm::vec3>("lightCol", this->lightCol);
        this->set_uniform<glm::vec3>("ambient", this->ambient);

        this->set_uniform<glm::vec3>("diffuseCol", this->diffuseCol);
    }
private:
    glm::vec3 diffuseCol;
    glm::vec3 lightPos, lightCol;
    glm::vec3 ambient;
}; // end class HalfLambert
}; // end namespace Shaders
}; // end namespace Ebb

#endif // HALF_LAMBERT_HPP