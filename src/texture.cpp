#include <ebb/texture.hpp>
#include <stdio.h>
#include <ebb/shader.hpp>

Ebb::Texture::Texture(int width, int height, int sampling) {
    glGenTextures(1, &this->TEX);
    glBindTexture(GL_TEXTURE_2D, this->TEX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->TEX, 0);

        // depth buffer
    glGenRenderbuffers(1, &this->DEP);
    glBindRenderbuffer(GL_RENDERBUFFER, this->DEP);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->DEP);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error creating framebuffer.\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

    // ndc
static float quadVertices[] = {
    // Positions (texcoords are extracted later)
    -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f
};

static char *VERT = (char *)R"(
#version 330 core

layout (location = 0) in vec3 posNDC;

out vec2 uv;

void main() {
    gl_Position = vec4(posNDC, 1.0);
    uv = posNDC.xy * 0.5 + 0.5;
}
)";

static char *FRAG = (char *)R"(
#version 330 core

in vec2 uv;

out vec4 Color;
uniform sampler2D tex;

void main() {
    Color = texture(tex, uv);
}
)";

void Ebb::Texture::draw(Ebb::Math::mat4x4 view, Ebb::Math::mat4x4 proj) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindTexture(GL_TEXTURE_2D, this->TEX);
    glActiveTexture(GL_TEXTURE0);

    Ebb::ObjectShader *shader = new Ebb::ObjectShader(VERT, FRAG);
    shader->use();
    shader->uniform((char *)"tex", 0);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}