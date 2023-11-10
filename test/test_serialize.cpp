#include <ebb/basic.hpp>
#define WIDTH  2256
#define HEIGHT 1504

Ebb::Node *root;
Ebb::Object *suzanne;

int main(int argc, char **argv) {
    Ebb::Window *win = new Ebb::Window("Serialize test");
    root = new Ebb::Node(nullptr);  // root of the node tree
        // Multiplying width and height by 2 gives pretty cheap antialiasing (due to linear texture filtering from the camera texture to the screen)
    Ebb::Camera *cam = new Ebb::Camera(root, WIDTH*2, HEIGHT*2, 60, glm::vec3(0.5294117647f, 0.8078431373f, 0.9215686275f));  // create a camera (FOV 60, sky blue background)
    cam->transform.translate(glm::vec3(0, 0, -3));  // scooch back
    Ebb::Mesh *obj = new Ebb::Mesh("models/suzanne.ebbm");  // load in a  mesh
    suzanne = new Ebb::Object(root);  // empty object to house suzanne

        // TODO: figure out why the light's z-axis is inverted (feature not bug ... right guys?)
    Ebb::Component *mesh = new Ebb::MeshRenderer(suzanne, new Ebb::Shaders::BlinnPhong(  // mesh renderer for suzanne, with a shiny blue material
        glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f), glm::vec3(0.5294117647f, 0.8078431373f, 0.9215686275f) * .1f,
        1.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), 64.0f
    ), obj);
    suzanne->transform.translation(glm::vec3(0.0, 0.0, 2.0));

    FILE *file = fopen("data/test_serialize.ebb", "wb");
    if (!file) {
        Ebb::runtime_error(true, "Failed to open node tree file.\n");
    }
    root->save(file);
    fclose(file);

    return 0;
}