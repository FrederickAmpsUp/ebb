#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Ebb {
/**
 * @brief Wrapper around GLFW3 window.
*/
class Window {
public:
    /**
     * @brief Function type for loop callbacks. This is called every frame.
    */
    typedef void (*RenderLoopCallback)(void);

    /**
     * @brief Creates a new Window with the given width, height and title.
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     * @param title The title of the window.
    */
    Window(int width, int height, const char *title);
    Window(int width, int height, const std::string& title);

    Window(int width, int height, const char *title,        RenderLoopCallback callback);
    Window(int width, int height, const std::string& title, RenderLoopCallback callback);

    /**
     * @brief Creates a new Window in fulscreen with the given title.
    */
    Window(const char *title);
    Window(const std::string& title);

    Window(const char *title,        RenderLoopCallback callback);
    Window(const std::string& title, RenderLoopCallback callback);

    /**
     * This is used internally, do not call it.
    */
    void _window_resize_callback(GLFWwindow* window, int width, int height);

    /**
     * @brief Get the width of the window
     * @return The width of the window
    */
    int get_width()  const { return width;  }

    /**
     * @brief Get the height of the window
     * @return The height of the window
    */
    int get_height() const { return height; }

    /**
     * @brief Get a handle to the internal GLFWwindow
     * @return A pointer to the internal GLFWwindow
    */
    GLFWwindow *get_window() const { return window; }

    /**
     * @brief Set the render loop callback
     * @param callback The render loop callback to set
    */
   void set_render_loop_callback(RenderLoopCallback callback) { render_loop_callback = callback; }

    /**
     * @brief Run the window's render loop.
    */
    void run(unsigned long long frames=-1);

    /**
     * @brief Run a single frame of the window's render loop.
    */
    void step();

    /**
     * @brief Make the window's context the active OpenGL context
    */
    void make_active();

private:
    int width, height;
    GLFWwindow* window;
    Window::RenderLoopCallback render_loop_callback;
}; // end class Window
}; // end namespace Ebb


#endif // WINDOW_HPP