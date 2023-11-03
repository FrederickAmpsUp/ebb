#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <ebb/error.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ebb {

class Shader {
public:
    /**
     * @brief Shader constructor.
     * This compiles and links the shader program.
     * @param vtx_src A C-string containing the vertex shader source.
     * @param frg_src A C-string containing the fragment shader source.
    */
    Shader(const char *vtx_src, 
           const char *frg_src) {

        this->_program_id = glCreateProgram();
        
        this->_vsh_id = glCreateShader(GL_VERTEX_SHADER  );
        this->_fsh_id = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(this->_vsh_id, 1, &vtx_src, NULL);
        glShaderSource(this->_fsh_id, 1, &frg_src, NULL);
        glCompileShader(this->_vsh_id);
        glCompileShader(this->_fsh_id);

        int success;
        glGetShaderiv(this->_vsh_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(this->_vsh_id, 1024, NULL, infoLog);
            Ebb::runtime_error(false, "Error compiling vertex shader: \n\n%s\n\n", infoLog);
        }

        glGetShaderiv(this->_fsh_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(this->_fsh_id, 1024, NULL, infoLog);
            Ebb::runtime_error(false, "Error compiling fragment shader: \n\n%s\n\n", infoLog);
        }

        glAttachShader(this->_program_id, this->_vsh_id);
        glAttachShader(this->_program_id, this->_fsh_id);
        glLinkProgram(this->_program_id);

        glGetProgramiv(this->_program_id, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(this->_program_id, 1024, NULL, infoLog);
            Ebb::runtime_error(false, "Error linking shader program: \n\n%s\n\n", infoLog);
        }
    }

    /**
     * @brief Set a shader uniform by name.
     * @tparam T The type of the uniform. This may raise an error if the type is not supported.
     * @param name The name of the shader uniform.
     * @param value The value to set.
    */
template <typename T>
    void set_uniform(const char *name, T value) {
        int location = glGetUniformLocation(this->_program_id, name);
        if (location != -1) {
            this->use();
            _uniform(location, value);
        } else {
            Ebb::runtime_error("Unable to find uniform %s\n", name);
        }
    }

    /**
     * @brief Make the compiled program active.
    */
    void use() {
        glUseProgram(this->_program_id);
    }
private:
    unsigned int _program_id;
    unsigned int _vsh_id, _fsh_id;

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
    void _unform(int location, glm::mat4x4 value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    // TODO: more uniform types (implement as needed)
}; // end class Shader
}; // end namespace Ebb

#endif // SHADER_HPP