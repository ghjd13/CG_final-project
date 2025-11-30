#include "globals.h"

namespace Game {
    void passiveMotion(int x, int y) {
        static float lastX = windowWidth / 2.0f;
        static float lastY = windowHeight / 2.0f;

        if (isCameraMode) {
            float xoffset = x - lastX;
            float yoffset = lastY - y;
            lastX = (float)x;
            lastY = (float)y;
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            cameraYaw += xoffset;
            cameraPitch += yoffset;
            if (cameraPitch > 89.0f) cameraPitch = 89.0f;
            if (cameraPitch < -89.0f) cameraPitch = -89.0f;
            glm::vec3 front;
            front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
            front.y = sin(glm::radians(cameraPitch));
            front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
            cameraFront = glm::normalize(front);
        }
        else {
            if (isFired || isResetting) return;
            float xOffset = (float)(x - windowWidth / 2) * 0.1f;
            float yOffset = (float)(windowHeight / 2 - y) * 0.1f;
            if (xOffset > 60.0f) xOffset = 60.0f;
            if (xOffset < -60.0f) xOffset = -60.0f;
            if (yOffset > 45.0f) yOffset = 45.0f;
            if (yOffset < -45.0f) yOffset = -45.0f;
            currentDartAim.x = yOffset;
            currentDartAim.y = xOffset;
            lastX = (float)x;
            lastY = (float)y;
        }
    }
}