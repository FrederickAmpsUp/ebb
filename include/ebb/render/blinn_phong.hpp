#ifndef BLINN_PHONG_HPP
#define BLINN_PHONG_HPP

#include <ebb/render/shader.hpp>
#include <ebb/internal/internals.hpp>
#include <ebb/render/camera.hpp>
#include <ebb/external/glm/glm.hpp>

static const char * const _blinn_vsh_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 objectMatrix;
uniform mat4 cameraMatrix;

mat3 getNormalMatrix(mat4 modelMatrix) {
    return mat3(transpose(modelMatrix));
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
    position = (vec4(aPos, 1.0) * objectMatrix).xyz;
}
)";

static const char * const _blinn_fsh_src = R"(
#version 330 core
varying vec3 normal;
varying vec3 position;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 ambient;
uniform float diffuseIntensity;
uniform float specularIntensity;

uniform vec3 cameraPos;

uniform vec3 diffuseCol;
uniform vec3 specularCol;
uniform float shininess;

layout (location = 0) out vec3 color;

void main() {
    vec3 lightDir = normalize(lightPos - position);
    vec3 viewDir = normalize(position - cameraPos);
    vec3 halfVec = normalize(lightDir + viewDir);

    float diffuseTerm = max(dot(normal, lightDir), 0.0);
    float specularTerm = pow(max(dot(normal, halfVec), 0.0), shininess);

    vec3 diffuseColor = diffuseCol * lightCol * diffuseIntensity;
    vec3 specularColor = specularCol * lightCol * specularIntensity;

    color = ambient + (diffuseTerm * diffuseColor) + (specularTerm * specularColor);
}
)";

namespace Ebb {

namespace Shaders {

/**
 * @brief Blinn-Phong lighting model for PB diffuse and specular lighting
 * See https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model for more information.
 * Overview:
 *  diffuse = dot(norm, dirToLight) * diffuseCol * lightCol
 *  half = normalize(dirToCam + dirToLight)
 *  specular = (dot(norm, half)^shininess) * specularCol * lightCol
 *  color = ambientCol + diffuse + specular
*/
class BlinnPhong : public Ebb::Shader {
public:
    BlinnPhong(
        glm::vec3 lightPos, glm::vec3 lightCol, glm::vec3 ambient,
        float diffuseIntensity, float specularIntensity,
        glm::vec3 diffuseCol, glm::vec3 specularCol,
        float shininess
    ) : Shader(_blinn_vsh_src, _blinn_fsh_src) {
        this->lightPos = lightPos;
        this->lightCol = lightCol;
        this->ambient = ambient;

        this->diffuseIntensity = diffuseIntensity;
        this->specularIntensity = specularIntensity;

        this->diffuseCol = diffuseCol;
        this->specularCol = specularCol;

        this->shininess = shininess;
    }

    void use() override {
        this->Ebb::Shader::use();
        this->set_uniform<glm::vec3>("lightPos", this->lightPos);
        this->set_uniform<glm::vec3>("lightCol", this->lightCol);
        this->set_uniform<glm::vec3>("ambient",  this->ambient );

        this->set_uniform<float>("diffuseIntensity",  this-> diffuseIntensity);
        this->set_uniform<float>("specularIntensity", this->specularIntensity);

        this->set_uniform<glm::vec3>("diffuseCol",  this-> diffuseCol);
        this->set_uniform<glm::vec3>("specularCol", this->specularCol);

        this->set_uniform<float>("shininess", this->shininess);

        this->set_uniform<glm::vec3>("cameraPos", _get_camera_pos());
    }
private:
    glm::vec3 lightPos, lightCol, ambient;
    float diffuseIntensity, specularIntensity;

    glm::vec3 diffuseCol, specularCol;
    float shininess;

    glm::vec3 _get_camera_pos() {
        return Ebb::Internals::activeCamera->transform.position();
    }
};
}; // end namespace Shader
}; // end namespace Ebb

#endif // BLINN_PHONG_HPP