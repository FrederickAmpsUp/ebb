#ifndef RENDERTEX_HPP
#define RENDERTEX_HPP

#include <ebb/render/renderable.hpp>
#include <ebb/render/shader.hpp>
#include <ebb/internal/classutil.hpp>

namespace Ebb {

class RenderTexture : public Ebb::Renderable {
public:
    /**
     * @brief Create a new render texture.
     * @param width The width of the texture.
     * @param height The height of the texture.
    */
    RenderTexture(Ebb::Node *parent, unsigned int width, unsigned int height);

    /**
     * @brief Bind the render texture.
    */
    void bind();

    /**
     * @brief Draw the render texture to the screen.
    */
    void draw() override;
private:
    unsigned int _fb_name;
    unsigned int _buf,_z_buf;

    unsigned int _width,_height;
    getter(_fb_name)
    getter(_buf)
    getter(_z_buf)
    getter(_width)
    getter(_height)
    
    Ebb::Shader __shader;
    unsigned int __VAO,__VBO;
};  // end class RenderTexture
};  // end namespace Ebb

#include <ebb/internal/cleanup.hpp>
#endif // RENDERTEX_HPP