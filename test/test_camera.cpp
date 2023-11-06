#include <ebb/basic.hpp>

#define WIDTH  800
#define HEIGHT 600

Ebb::Node *root;
Ebb::Window *win;
void frame_callback() {
    if (win->key_pressed(GLFW_KEY_ESCAPE)) {
        win->close();
    }

    root->update();
    win->clear(glm::vec3(1.0));
    root->get_child<Ebb::Camera>()->get_tex()->draw();
}

int main(int argc, char **argv) {
    win = new Ebb::Window(WIDTH, HEIGHT, "Camera test", &frame_callback);

    root = new Ebb::Node(nullptr);
        // Multiplying width and height by 2 gives pretty cheap antialiasing (due to linear texture filtering from the camera texture to the screen)
    Ebb::Camera *cam = new Ebb::Camera(root, WIDTH*2, HEIGHT*2, 60);
    cam->transform.translate(glm::vec3(0, 0, -3));
    Ebb::Mesh *obj = new Ebb::Mesh("models/suzanne.ebbm");
    Ebb::Object *suzanne = new Ebb::Object(root);

        // TODO: figure out why the light's z-axis is inverted
    Ebb::Component *mesh = new Ebb::MeshRenderer(suzanne, new Ebb::Shaders::BlinnPhong(
        glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f), glm::vec3(.1f),
        1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), 64.0f
    ), obj);

    win->run();
}