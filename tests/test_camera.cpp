#include <ebb/camera.hpp>
#include <ebb/node.hpp>
#include <ebb/mesh/mesh.hpp>
#include <ebb/transform.hpp>
#include <ebb/math.hpp>
#include <ebb/window.hpp>
#include <ebb/mesh/renderer.hpp>
#include <GL/glut.h>

class CamTexDrawer : public Ebb::NodeManager {
public:
    void setup() {}
    void update() {
        win->clear();
        cam->texture()->draw();
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            printf("%s\n", gluErrorString(glGetError()));
    }

    Ebb::Window *win;
    Ebb::Camera *cam;
};

class Rotate : public Ebb::Node {
public:
    using Ebb::Node::Node;

    void update() override {
        object->rotateY(0.01);
    }

    Ebb::Transform *object;

private:
};

int main(int argc, char **argv) {
    Ebb::Window::setup();
    Ebb::Node *root = new Ebb::Node;
    Ebb::WindowManager *win = new Ebb::WindowManager(root, new Ebb::Window(1920, 1080, "Test window"));
    Ebb::NodeTreeManager *manager = new Ebb::NodeTreeManager(root);

    Ebb::Transform *camera = new Ebb::Transform(root);
    Ebb::Camera *_cam = new Ebb::Camera(camera, Ebb::Math::radians(80.0f), 1920, 1080);

    Ebb::Transform *mesh = new Ebb::Transform(root);
    mesh->translate(Ebb::Math::vec3(0.0f, 0.0f, 2.0f));
    Ebb::MeshRenderer *meshRender = new Ebb::MeshRenderer(mesh, Ebb::Mesh::loadOBJ("models/monke-smooth.obj"));

    CamTexDrawer *drawer = new CamTexDrawer();
    drawer->win = win->window();
    drawer->cam = _cam;
    manager->addManager(drawer);

    Rotate *r = new Rotate(mesh);
    r->object = mesh;

    manager->run();

    Ebb::Window::teardown();
    return 0;
}