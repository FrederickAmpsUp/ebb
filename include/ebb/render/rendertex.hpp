#ifndef RENDERTEX_HPP
#define RENDERTEX_HPP

#include <ebb/render/renderable.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/external/glm/glm.hpp>
#include <stdio.h>
#include <new>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class RenderTexture : public Ebb::Renderable {
public:
    /**
     * @brief Create a new render texture.
     * @param width The width of the texture.
     * @param height The height of the texture.
    */
    RenderTexture(Ebb::Node *parent, unsigned int width=800, unsigned int height=600);

    /**
     * @brief Bind the render texture.
    */
    void bind();

    /**
     * @brief Draw the render texture to the screen.
    */
    void draw() override;

    /**
     * @brief Clear the render texture.
    */
    void clear();

    /**
     * @brief Clear the render texture to the specified color.
    */
    void clear(glm::vec3 color);

    virtual void save(FILE *file) override {
        this->Renderable::save(file);
        fwrite(&this->_width, sizeof(unsigned int), 1, file);
        fwrite(&this->_height, sizeof(unsigned int), 1, file);  // Everything else is dynamically generated at constructor
    }
    virtual void load(FILE *file) override {
        this->Renderable::load(file);
        fread(&this->_width, sizeof(unsigned int), 1, file);
        fread(&this->_height, sizeof(unsigned int), 1, file);  // generate shader and framebuffers
        new (this) RenderTexture(this->get_parent(), this->_width, this->_height);
    }
    virtual Node *construct(Node *parent) override {
        return new RenderTexture(parent);
    }
private:
    unsigned int _fb_name;
    unsigned int _buf,_z_buf;

    unsigned int _width,_height;
    getter(_fb_name)
    getter(_buf)
    getter(_z_buf)
    getter(_width)
    getter(_height)
    
    Ebb::Shader *__shader;
    unsigned int __VAO,__VBO;
};  // end class RenderTexture
};  // end namespace Ebb

#include <ebb/internal/cleanup.hpp>
#endif // RENDERTEX_HPP