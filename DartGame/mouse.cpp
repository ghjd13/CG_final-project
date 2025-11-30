#define _CRT_SECURE_NO_WARNINGS
#include "globals.h"

namespace Game {
    void mouse(int button, int state, int x, int y) {
        if (isCameraMode) return;
        if (matchOver) return; // 매치 종료 시 발사 금지

        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (!isFired && !isResetting) {
                // 각 플레이어가 세트 내 던질 수 있는 최대 횟수(5회) 체크
                if ((currentPlayer == 1 && player1Throws >= 5) ||
                    (currentPlayer == 2 && player2Throws >= 5)) {
                    snprintf(scoreMessage, sizeof(scoreMessage), "Player %d has no throws left this set.", currentPlayer);
                    return;
                }

                isFired = true;
                dartTime = 0.0f;
                dartLaunchPos = dartWorldPos;
                float pitch = glm::radians(currentDartAim.x);
                float yaw = glm::radians(currentDartAim.y);
                glm::vec3 dir;
                dir.x = sin(yaw) * cos(pitch);
                dir.y = sin(pitch);
                dir.z = -cos(yaw) * cos(pitch);
                float power = 25.0f;
                dartInitialVel = glm::normalize(dir) * power;

                activeThrowPlayer = currentPlayer; // 누가 던졌는지 기록
                snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Flying...", activeThrowPlayer);
            }
        }
    }
}