#pragma once

#include <glad/glad.h>
#include <ebb/math.hpp>
#include <ebb/texture.hpp>

namespace Ebb {
class ObjectShader {
public:
    ObjectShader();
    ObjectShader(const char *frag);
    ObjectShader(const char *vert, const char *frag);

    void use() {
        glUseProgram(this->program);
    }

    void uniform(char *name, int value);
    void uniform(char *name, float value);
    void uniform(char *name, Ebb::Math::vec2 value);
    void uniform(char *name, Ebb::Math::vec3 value);
    void uniform(char *name, Ebb::Math::vec4 value);
    void uniform(char *name, Ebb::Math::mat2x2 value);
    void uniform(char *name, Ebb::Math::mat3x3 value);
    void uniform(char *name, Ebb::Math::mat4x4 value);
private:
    unsigned int vert,frag,program;
};
};