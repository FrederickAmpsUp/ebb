#include <ebb/basic.hpp>
#define WIDTH  2256
#define HEIGHT 1504

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
    Ebb::Node::register_type<Ebb::Node>();
    Ebb::Node::register_type<Ebb::Object>();
    Ebb::Node::register_type<Ebb::Component>();
    Ebb::Node::register_type<Ebb::MeshRenderer>();
    Ebb::Node::register_type<Ebb::Renderable>();
    Ebb::Node::register_type<Ebb::RenderTexture>();
    Ebb::Node::register_type<Ebb::Camera>();
    
    FILE *f = fopen("data/test_serialize.ebb", "rb");
    if (!f) {
        Ebb::runtime_error(true, "Couldn't open node tree file.\n");
    }
    root->load(f);
    fclose(f);
    printf("%d\n", root->get_children().size());

    glm::vec3 pos = root->get_child<Ebb::Camera>()->transform.position();
    printf("%f %f %f\n", pos.x, pos.y, pos.z);

    suzanne = root->get_child<Ebb::Object>();
    if (dynamic_cast<Ebb::Camera *>(suzanne)) suzanne = root->get_child<Ebb::Object>(1);

    printf("%f %f %f\n", suzanne->transform.position().x, suzanne->transform.position().y, suzanne->transform.position().z);

    root->setup();  // setup the node tree

    win->run(); // run the windw

    return 0; // exit
}