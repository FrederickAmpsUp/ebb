#include <ebb/render/rendertex.hpp>
#include <glad/glad.h>
#include <GL/glu.h>

static const char * const rtex_vert_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

varying vec2 uv;
void main() {
    uv = aPos.xy * 0.5 + 0.5;
    gl_Position = vec4(aPos, 1.0);
}
)";

static const char * const rtex_frag_src = R"(
#version 330 core
varying vec2 uv;
uniform sampler2D _texture;
void main() {
    gl_FragColor = texture(_texture, uv);
}
)";

Ebb::RenderTexture::RenderTexture(Ebb::Node *parent, unsigned int width, unsigned int height) : Ebb::Renderable(parent) {
    this->__shader = new Ebb::Shader(rtex_vert_src, rtex_frag_src);
    this->_width  =  width;
    this->_height = height;

    glGenFramebuffers(1, &this->_fb_name);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fb_name);

    glGenTextures(1, &this->_buf);
    glBindTexture(GL_TEXTURE_2D, this->_buf);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_width, this->_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_buf, 0);

    glGenRenderbuffers(1, &this->_z_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_z_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_z_buf);

    glGenVertexArrays(1, &this->__VAO);
    glBindVertexArray(this->__VAO);

    glGenBuffers(1, &this->__VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->__VBO);

    const float fullscreen_quad[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreen_quad), fullscreen_quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glViewport(0,0, width,height);

}

void Ebb::RenderTexture::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->_fb_name);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glViewport(0,0, this->_width, this->_height);
}

void Ebb::RenderTexture::draw() {
    glBindVertexArray(this->__VAO);
    this->__shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_buf);
    this->__shader->set_uniform<int>("_texture", 0);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

void Ebb::RenderTexture::clear() {
    this->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Ebb::RenderTexture::clear(glm::vec3 color) {
    this->bind();
    glClearColor(color.x, color.y, color.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}