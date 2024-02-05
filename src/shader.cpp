#include <ebb/shader.hpp>
#include <stdio.h>

    // default vertex shader, may load this from a file in the future
static char *vertexSource = (char *)R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos; // For passing position to the fragment shader
out vec3 Normal;  // For passing normals to the fragment shader

uniform mat4 model;      // Object's model matrix
uniform mat4 view;       // Camera's view matrix
uniform mat4 projection; // Projection matrix

void main()
{
    // Transform the vertex position
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    vec4 viewPosition = view * worldPosition;
    gl_Position = projection * viewPosition;
    gl_Position = vec4(aPosition, 1.0);

    // Pass position and normal to the fragment shader
    FragPos = worldPosition.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space
}
)";

    // default fragment shader
static char *fragmentSource = (char *)R"(
#version 330 core

in vec3 FragPos;
out vec4 Color;

void main()
{
    Color = vec4(1.0, 0.0, 1.0, 1.0); // default, doesn't need to be fancy
}
)";

static void _shaderErr(unsigned int id, GLenum flag) {
    char infoLog[1024];
    int success;
    glGetShaderiv(id, flag, &success);

    if (!success) {
        glGetShaderInfoLog(id, sizeof(infoLog) / sizeof(char), nullptr, infoLog);
        fprintf(stderr, "Error compiling shader: \n%s\n\n", infoLog);
    }
}
static void _programErr(unsigned int id, GLenum flag) {
    char infoLog[1024];
    int success;
    glGetProgramiv(id, flag, &success);

    if (!success) {
        glGetProgramInfoLog(id, sizeof(infoLog) / sizeof(char), nullptr, infoLog);
        fprintf(stderr, "Error linking shader program: \n%s\n\n", infoLog);
    }
}

Ebb::ObjectShader::ObjectShader() : ObjectShader(vertexSource, fragmentSource) {}

Ebb::ObjectShader::ObjectShader(const char *frag) : ObjectShader(vertexSource, frag) {}

Ebb::ObjectShader::ObjectShader(const char *vert, const char *frag) {
    this->vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vert, 1, &vert, nullptr);
    glCompileShader(this->vert);
    _shaderErr(this->vert, GL_COMPILE_STATUS);

    this->frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->frag, 1, &frag, nullptr);
    glCompileShader(this->frag);
    _shaderErr(this->frag, GL_COMPILE_STATUS);

    this->program = glCreateProgram();
    glAttachShader(this->program, this->vert);
    glAttachShader(this->program, this->frag);
    glLinkProgram(this->program);
    _programErr(this->program, GL_LINK_STATUS);

    glDeleteShader(this->vert);
    glDeleteShader(this->frag);
}