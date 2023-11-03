#include <ebb/core/window.hpp>
#include <stdio.h>

void frame_callback() {
    printf("frame lol\n");
}

int main(int argc, char **argv) {
    Ebb::Window *win = new Ebb::Window(800,600, "Test window", frame_callback);

    win->run();
    return 0;
}