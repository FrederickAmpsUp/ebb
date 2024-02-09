#pragma once

#include <glad/glad.h>
#include <ebb/renderer.hpp>

namespace Ebb {
class Texture : public Ebb::Renderable {
public:
    Texture(int width, int height, int sampling=GL_LINEAR);

    void bind() {
        glBindTexture(GL_TEXTURE_2D, this->TEX);
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    }

    void draw(Ebb::Math::mat4x4 view=Ebb::Math::mat4x4(1.0), Ebb::Math::mat4x4 proj=Ebb::Math::mat4x4(1.0));
    void clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    int width() {
        int w;
        glBindTexture(GL_TEXTURE_2D, this->TEX);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &w);
        return w;
    }
    int height() {
        int h;
        glBindTexture(GL_TEXTURE_2D, this->TEX);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &h);
        return h;
    }
private:
    unsigned int FBO, TEX, DEP;
};
};