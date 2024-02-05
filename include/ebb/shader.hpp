#pragma once

#include <glad/glad.h>

namespace Ebb {
class ObjectShader {
public:
    ObjectShader();
    ObjectShader(const char *frag);
    ObjectShader(const char *vert, const char *frag);

    void use() {
        glUseProgram(this->program);
    }
private:
    unsigned int vert,frag,program;
};
};