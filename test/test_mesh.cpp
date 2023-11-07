#include <ebb/basic.hpp>
#include <stdio.h>

// borken test :skull:
// you served your purpose, now i've moved on to test_camera.hpp

Ebb::Window *win;
Ebb::Object *suzanne;
Ebb::Shader *shader;
Ebb::RenderTexture *tex;

void frame_callback() {
    win->make_active();
        // Close the window when ESC is pressed
    if (win->key_pressed(GLFW_KEY_ESCAPE)) {
        win->close();
    }
    win->clear(glm::vec3(0.0));
    tex->bind();
    suzanne->get_child<Ebb::MeshRenderer>()->draw();
    win->make_active();
    tex->draw();
}

int main(int argc, char **argv) {
    win = new Ebb::Window(800, 800, "Test mesh", frame_callback); // Create the window

    Ebb::Mesh *mesh = new Ebb::Mesh("models/suzanne.ebbm");  // Load in Suzanne the Blender monkey from my models/ folder
    Ebb::Node *root = new Ebb::Node(nullptr);
    tex = new Ebb::RenderTexture(root, 800*2,800*2);
    

        // PBR shader
    shader = new Ebb::Shaders::BlinnPhong(  // Basic shiny white material
        glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(.9f), glm::vec3(.1f),
        1.0f, 1.0f, glm::vec3(.9f), glm::vec3(.95f), 32.0f
    );
/*
        // NON-PBR shader
    shader = new Ebb::Shaders::HalfLambert(  // Basic white material
        glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(.9f), glm::vec3(.1f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    );*/
    suzanne = new Ebb::Object(root); // empty object
    suzanne->transform.rotate(glm::vec3(0.0f, 0.0f, 0.0f));  // rotate 180 around y-axis (Y is up eat it)
    Ebb::MeshRenderer *renderer = new Ebb::MeshRenderer(suzanne, shader, mesh);  // add a mesh renderer to render the mesh

    Ebb::Internals::activeCamera = new Ebb::Camera(nullptr, 1,1,1.0f);  // segfault if not set
    Ebb::Internals::activeCamera->transform.translate(glm::vec3(0,0,-100));

    win->make_active();
    win->run(); // Run the window. No frame count is specified, the window will run forever
 
    return 0;
}