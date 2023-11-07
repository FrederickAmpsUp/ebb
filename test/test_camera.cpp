#include <ebb/basic.hpp>

#define WIDTH  800
#define HEIGHT 600

Ebb::Node *root;
Ebb::Window *win;
Ebb::Object *suzanne;
void frame_callback() {
    if (win->key_pressed(GLFW_KEY_ESCAPE)) {
        win->close();  // close the window when escape is pressed
    }

    root->update();  // update the node tree
    win->clear(glm::vec3(1.0));  // clear the window
    root->get_child<Ebb::Camera>()->get_tex()->draw();  // draw the camera's render texture to the window

    suzanne->transform.translation(glm::vec3(glm::sin(Ebb::Time::elapsed_seconds()), 0.0f, 0.0f));
    suzanne->transform.rotation(glm::vec3(0.0, glm::degrees(Ebb::Time::elapsed_seconds()), 0.0));
}

int main(int argc, char **argv) {
    win = new Ebb::Window(WIDTH, HEIGHT, "Camera test", &frame_callback);  // create a window

    root = new Ebb::Node(nullptr);  // root of the node tree
        // Multiplying width and height by 2 gives pretty cheap antialiasing (due to linear texture filtering from the camera texture to the screen)
    Ebb::Camera *cam = new Ebb::Camera(root, WIDTH*2, HEIGHT*2, 60);  // create a camera (FOV 60)
    cam->transform.translate(glm::vec3(0, 0, -3));  // scooch back
    Ebb::Mesh *obj = new Ebb::Mesh("models/suzanne.ebbm");  // load in a  mesh
    suzanne = new Ebb::Object(root);  // empty object to house suzanne

        // TODO: figure out why the light's z-axis is inverted (feature not bug ... right guys?)
    Ebb::Component *mesh = new Ebb::MeshRenderer(suzanne, new Ebb::Shaders::BlinnPhong(  // mesh renderer for suzanne, with a shiny blue material
        glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f), glm::vec3(.1f),
        1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), 64.0f
    ), obj);

    root->setup();  // setup the node tree

    win->run(); // run the windw

    return 0; // exit
}