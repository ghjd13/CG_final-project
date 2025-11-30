#include "callbacks.h"
#include "globals.h"

namespace Game {
    void reshape(int w, int h) {
        windowWidth = w;
        windowHeight = h;
        glViewport(0, 0, w, h);
    }
}