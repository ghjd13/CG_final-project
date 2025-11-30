#include "globals.h"

namespace Game {
    void keyboard(unsigned char key, int x, int y) {
        float cameraSpeed = 10.0f * deltaTime;

        if (key == 'c' || key == 'C') {
            isCameraMode = !isCameraMode;
            if (!isCameraMode) {
                cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
                cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                cameraYaw = -90.0f;
                cameraPitch = 0.0f;
            }
        }

        if (isCameraMode) {
            if (key == 'w' || key == 'W') cameraPos += cameraSpeed * cameraFront;
            if (key == 's' || key == 'S') cameraPos -= cameraSpeed * cameraFront;
            if (key == 'a' || key == 'A') cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (key == 'd' || key == 'D') cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if (key == 27) exit(0);
    }
}