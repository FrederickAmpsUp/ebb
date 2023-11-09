#include <ebb/basic.hpp>
#include <ebb/data/loader.hpp>

#define WIDTH  2256
#define HEIGHT 1504

typedef enum {
    NODE,
    OBJECT,
    MESH_RENDERER,
    CAMERA
} UsedTypes;

Ebb::Node *Ebb::Data::instantiate_node(int type, Node *parent) {
    switch (type) {
        case NODE:
            return new Ebb::Node(parent);
        case OBJECT:
            return new Ebb::Object(parent);
        case MESH_RENDERER:
            return new Ebb::MeshRenderer(parent);
        case CAMERA:
            return new Ebb::Camera(parent);
        default:
            Ebb::runtime_error(true, "Invalid node type.\n");
    }
    return nullptr;
}

int Ebb::Data::node_type(Node *node) {
    if (dynamic_cast<Ebb::Camera *>(node))       return CAMERA;
    if (dynamic_cast<Ebb::MeshRenderer *>(node)) return MESH_RENDERER;
    if (dynamic_cast<Ebb::Object *>(node))       return OBJECT;
    if (dynamic_cast<Ebb::Node *>(node))         return NODE;

    Ebb::runtime_error(true, "Invalid node type.\n");
    return 0;
}

Ebb::Node *root;
Ebb::Window *win;
Ebb::Object *suzanne;
void frame_callback() {
    if (win->key_pressed(EBB_KEY(ESCAPE))) {
        win->close();  // close the window when escape is pressed
    }

    root->update();  // update the node tree
    //win->clear(glm::vec3(1.0));  // clear the window
    win->make_active(); // make the window active, camera update binds the camera's render texture
    root->get_child<Ebb::Camera>()->get_tex()->draw();  // draw the camera's render texture to the window

    //suzanne->transform.translation(glm::vec3(glm::sin(Ebb::Time::elapsed_seconds()), 0.0f, glm::cos(Ebb::Time::elapsed_seconds())));
    suzanne->transform.rotation(glm::vec3(0.0, glm::degrees(Ebb::Time::elapsed_seconds()), 0.0));
}

int main(int argc, char **argv) {
    win = new Ebb::Window("Serialization test", &frame_callback);  // create a window

    root = new Ebb::Node(nullptr);  // root of the node tree
    FILE *f = fopen("data/test_serialize.ebb", "rb");
    if (!f) {
        Ebb::runtime_error(true, "Couldn't open node tree file.\n");
    }
    root->load(f);
    fclose(f);

    root->setup();  // setup the node tree

    win->run(); // run the windw

    return 0; // exit
}