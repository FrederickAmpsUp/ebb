#include <ebb/render/shader.hpp>

Ebb::VertexShader::VertexShader(const char *src) {
    this->_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_id, 1, &src, NULL);
    glCompileShader(this->_id);

    int success;
    glGetShaderiv(this->_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(this->_id, 1024, NULL, infoLog);
        Ebb::runtime_error(false, "Error compiling vertex shader: \n\n%s\n\n", infoLog);
    }
}

Ebb::FragmentShader::FragmentShader(const char *src) {
    this->_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_id, 1, &src, NULL);
    glCompileShader(this->_id);

    int success;
    glGetShaderiv(this->_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(this->_id, 1024, NULL, infoLog);
        Ebb::runtime_error(false, "Error compiling vertex shader: \n\n%s\n\n", infoLog);
    }
}

Ebb::Shader::Shader(const char *vtx_src, const char *frg_src) {
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

Ebb::Shader::Shader(Ebb::VertexShader *vsh, Ebb::FragmentShader *fsh) {
    this->_program_id = glCreateProgram();
    this->_vsh_id = vsh->get_id();
    this->_fsh_id = fsh->get_id();

    glAttachShader(this->_program_id, this->_vsh_id);
    glAttachShader(this->_program_id, this->_fsh_id);

    glLinkProgram(this->_program_id);

    int success;
    glGetProgramiv(this->_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(this->_program_id, 1024, NULL, infoLog);
        Ebb::runtime_error(false, "Error linking shader program: \n\n%s\n\n", infoLog);
    }
}