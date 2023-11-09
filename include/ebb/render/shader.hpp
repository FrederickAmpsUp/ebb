#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <ebb/error.hpp>
#include <ebb/external/glm/glm.hpp>
#include <ebb/external/glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <new>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class VertexShader {
public:
    /**
     * @brief Vertex shader constructor.
     * This compiles the shader.
     * @param src A C-string containing the shader source.
    */
    VertexShader(const char *src="");

    virtual void save(FILE *file) {
        fputs(this->_src, file);
        fputc('\0', file);
    }
    virtual void load(FILE *file) {
        long startPos = ftell(file);
        char c;
        while ((c = fgetc(file)));
        long endPos = ftell(file);
        fseek(file, startPos, SEEK_SET);
        int stringLength = endPos - startPos;

        char *buf = (char *)malloc(stringLength);
        fread(buf, stringLength, 1, file);
        new (this) VertexShader(buf);
    }
private:
    unsigned int _id;
    getter(_id)

    char *_src;
    getter(_src)
};

class FragmentShader {
public:
    /**
     * @brief Fragment shader constructor.
     * This compiles the shader.
     * @param src A C-string containing the shader source.
    */
    FragmentShader(const char *src="");

    virtual void save(FILE *file) {
        fputs(this->_src, file);
        fputc('\0', file);
    }
    virtual void load(FILE *file) {
        long startPos = ftell(file);
        char c;
        while ((c = fgetc(file)));
        long endPos = ftell(file);
        fseek(file, startPos, SEEK_SET);
        int stringLength = endPos - startPos;

        char *buf = (char *)malloc(stringLength);
        fread(buf, stringLength, 1, file);
        new (this) FragmentShader(buf);
    }
private:
    unsigned int _id;
    getter(_id)

    char *_src;
    getter(_src)
};

class Shader {
public:
    /**
     * @brief Shader constructor.
     * This compiles and links the shader program.
     * @param vtx_src A C-string containing the vertex shader source.
     * @param frg_src A C-string containing the fragment shader source.
    */
    Shader(const char *vtx_src="", 
           const char *frg_src="");

    /**
     * @brief Shader constructor.
     * This links the vert/frag shaders.
     * @param vsh The vertex shader to use.
     * @param fsh The fragment shader to use.
    */
    Shader(VertexShader   *vsh,
           FragmentShader *fsh);

    /**
     * @brief Set a shader uniform by name.
     * @tparam T The type of the uniform. This may raise an error if the type is not supported.
     * @param name The name of the shader uniform.
     * @param value The value to set.
    */
template <typename T>
    void set_uniform(const char *name, T value) {
        this->Ebb::Shader::use();
        int location = glGetUniformLocation(this->_program_id, name);
        if (location != -1) {
            _uniform(location, value);
        } else {
            Ebb::runtime_error(false, "Unable to find uniform %s\n", name);
        }
    }

    /**
     * @brief Make the compiled program active.
    */
    virtual void use() {
        glUseProgram(this->_program_id);
    }

    virtual void save(FILE *file) {
        this->_vsh->save(file);
        this->_fsh->save(file);
    }
    virtual void load(FILE *file) {
        this->_vsh->load(file);
        this->_fsh->load(file);
    }

private:
    unsigned int _program_id;
    getter(_program_id)
    unsigned int _vsh_id, _fsh_id;

    VertexShader *_vsh;
    FragmentShader *_fsh;
    getter(_vsh)
    getter(_fsh)

    void _uniform(int location, int value) {
        glUniform1i(location, value);
    }
    void _uniform(int location, float value) {
        glUniform1f(location, value);
    }
    void _uniform(int location, glm::vec2 value) {
        glUniform2f(location, value.x, value.y);
    }
    void _uniform(int location, glm::vec3 value) {
        glUniform3f(location, value.x, value.y, value.z);
    }
    void _uniform(int location, glm::mat4x4 value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    // TODO: more uniform types (implement as needed)
}; // end class Shader
}; // end namespace Ebb

#include <ebb/internal/cleanup.hpp>

#endif // SHADER_HPP