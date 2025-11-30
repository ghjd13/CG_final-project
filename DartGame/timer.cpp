#define _CRT_SECURE_NO_WARNINGS
#include "globals.h"
#include "callbacks.h"

namespace Game {
    void timer(int value) {
        float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        boardRotAngle += 100.0f * deltaTime;
        if (boardRotAngle > 360.0f) boardRotAngle -= 360.0f;

        // 기계 회전(전역 변수 참조)
        ::machineRotAngle += 20.0f * deltaTime;
        if (::machineRotAngle > 360.0f) ::machineRotAngle -= 360.0f;

        lightSwingAngle = sin(currentTime * 2.0f);
        swingingLightPos.x = sin(lightSwingAngle) * 8.0f;

        if (isResetting) {
            resetTimer += deltaTime;
            if (resetTimer >= 1.0f) {
                isResetting = false;
                resetTimer = 0.0f;
                dartWorldPos = dartStartPos;
                currentDartAim =   glm::vec3(0.0f);
                dartTime = 0.0f;

                // 라운드 전환: 리셋 완료 후 다음 플레이어로 교대 (단, 세트/매치 끝나면 처리)
                // 만약 현재 세트에서 양 플레이어가 모두 5회씩 던졌다면 세트 종료
                bool setFinished = (player1Throws >= 5 && player2Throws >= 5);
                if (setFinished) {
                    // 세트 승자 결정
                    if (player1SetPoints > player2SetPoints) {
                        player1SetsWon++;
                        snprintf(scoreMessage, sizeof(scoreMessage), "Set %d: Player 1 wins (%d - %d)", currentSetNumber, player1SetPoints, player2SetPoints);
                    } else if (player2SetPoints > player1SetPoints) {
                        player2SetsWon++;
                        snprintf(scoreMessage, sizeof(scoreMessage), "Set %d: Player 2 wins (%d - %d)", currentSetNumber, player2SetPoints, player1SetPoints);
                    } else {
                        // 동점
                        snprintf(scoreMessage, sizeof(scoreMessage), "Set %d: Draw (%d - %d)", currentSetNumber, player1SetPoints, player2SetPoints);
                    }

                    currentSetNumber++;
                    // 세트 초기화
                    player1SetPoints = 0;
                    player2SetPoints = 0;
                    player1Throws = 0;
                    player2Throws = 0;

                    // 한 세트 끝나면 붙어있던 모든 다트 제거
                    clearStuckDarts();

                    // 매치 종료 확인: 먼저 3세트 획득하면 승리
                    if (player1SetsWon >= 3 || player2SetsWon >= 3 || currentSetNumber > 5) {
                        matchOver = true;
                        if (player1SetsWon > player2SetsWon) {
                            snprintf(scoreMessage, sizeof(scoreMessage), "Match Over: Player 1 wins %d - %d", player1SetsWon, player2SetsWon);
                        } else if (player2SetsWon > player1SetsWon) {
                            snprintf(scoreMessage, sizeof(scoreMessage), "Match Over: Player 2 wins %d - %d", player2SetsWon, player1SetsWon);
                        } else {
                            snprintf(scoreMessage, sizeof(scoreMessage), "Match Over: Draw %d - %d", player1SetsWon, player2SetsWon);
                        }
                    } else {
                        // 다음 세트 시작: Player 1이 항상 먼저 시작
                        currentPlayer = 1;
                        activeThrowPlayer = 0;
                    }
                } else {
                    // 세트가 끝나지 않았다면 턴 교대
                    currentPlayer = (currentPlayer == 1) ? 2 : 1;
                    activeThrowPlayer = 0;
                    snprintf(scoreMessage, sizeof(scoreMessage), "Player %d's turn", currentPlayer);
                }
            }
        }
        else if (isFired) {
            dartTime += deltaTime;
            glm::vec3 gravity(0.0f, -3.0f, 0.0f);
            dartWorldPos = dartLaunchPos + (dartInitialVel * dartTime) + (0.5f * gravity * dartTime * dartTime);

            // ======================================
            // 1) 다트판 충돌 (로컬 좌표로 판정 ? 보드 외부(갈색)는 0점)
            // ======================================
            if (dartWorldPos.z < -9.5f) {
                // 보드 충돌 시 먼저 보드 로컬 좌표로 판정해 점수 결정
                // 보드 모델(위치/회전/스케일) ? 생성과 동일한 행렬
                glm::mat4 boardModel = glm::mat4(1.0f);
                boardModel = glm::translate(boardModel, glm::vec3(0.0f, 0.0f, -10.0f));
                boardModel = glm::rotate(boardModel, glm::radians(boardRotAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                boardModel = glm::scale(boardModel, glm::vec3(5.0f, 5.0f, 1.0f));

                // 충돌 지점을 보드 로컬 좌표로 변환
                glm::mat4 inv = glm::inverse(boardModel);
                glm::vec4 local4 = inv * glm::vec4(dartWorldPos, 1.0f);
                glm::vec3 local = glm::vec3(local4);

                // 로컬 거리 / 보드 메쉬 반지름(메쉬에서 사용한 radius = 0.5)
                float localDist = std::sqrt(local.x * local.x + local.y * local.y);
                const float boardMeshRadius = 0.5f;
                float nr = localDist / boardMeshRadius; // 정규화 반경

                int points = 0;

                if (nr > 1.0f) {
                    // 보드 바깥(갈색) -> 0점, 다트는 월드에 고정
                    points = 0;

                    isFired = false;
                    dartWorldPos.z = -9.5f;
                    {
                        StuckDart sd;
                        sd.attachedToBoard = false;
                        sd.scored = false;
                        sd.worldPos = dartWorldPos;
                        sd.worldDir = glm::normalize(dartInitialVel);
                        sd.player = activeThrowPlayer;
                        g_StuckDarts.push_back(sd);
                    }

                    isResetting = true;
                    // 던진 횟수 증가
                    if (activeThrowPlayer == 1) player1Throws++;
                    else if (activeThrowPlayer == 2) player2Throws++;

                    snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Miss (outside board)", activeThrowPlayer);
                } else {
                    // 보드 내부: 섹션/불즈 판정
                    float angle = atan2(local.y, local.x);
                    float angleDeg = glm::degrees(angle);
                    if (angleDeg < 0) angleDeg += 360.0f;

                    float relativeAngle = angleDeg;
                    while (relativeAngle < 0) relativeAngle += 360.0f;
                    while (relativeAngle >= 360.0f) relativeAngle -= 360.0f;

                    int section = (int)(relativeAngle / 30.0f) + 1;
                    points = section * 10;
                    if (nr < 0.04f) { // 불즈 조건 (조정 가능)
                        points = 200;
                    }

                    // scored 다트는 보드 로컬 좌표로 저장하여 보드와 함께 회전
                    isFired = false;
                    dartWorldPos.z = -9.5f;
                    {
                        StuckDart sd;
                        sd.attachedToBoard = true;
                        sd.scored = (points > 0);
                        sd.worldPos = dartWorldPos;
                        sd.worldDir = glm::normalize(dartInitialVel);
                        sd.localPos = local; // 로컬 좌표 저장
                        // localDir 계산 (회전만 역변환)
                        glm::mat3 boardRotMat = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(boardRotAngle), glm::vec3(0,0,1)));
                        sd.localDir = glm::transpose(boardRotMat) * glm::normalize(dartInitialVel);
                        sd.player = activeThrowPlayer;
                        g_StuckDarts.push_back(sd);
                    }

                    isResetting = true;

                    // 점수 반영 및 던진 횟수 증가
                    if (activeThrowPlayer == 1) {
                        player1SetPoints += points;
                        player1Throws++;
                    } else if (activeThrowPlayer == 2) {
                        player2SetPoints += points;
                        player2Throws++;
                    }

                    if (points == 0) snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Miss", activeThrowPlayer);
                    else snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Hit +%d (SetScore P1:%d P2:%d)", activeThrowPlayer, points, player1SetPoints, player2SetPoints);
                }
            }
            // ======================================
            // 2) 벽 충돌 처리: 이제 월드에 고정
            // ======================================
            else if (dartWorldPos.x > 19.5f || dartWorldPos.x < -19.5f || dartWorldPos.z < -19.5f) {
                isFired = false;

                glm::vec3 hitPos = dartWorldPos;
                if (dartWorldPos.x > 19.5f) hitPos.x = 19.5f;
                if (dartWorldPos.x < -19.5f) hitPos.x = -19.5f;
                if (dartWorldPos.z < -19.5f) hitPos.z = -19.5f;

                dartWorldPos = hitPos;

                // --- 저장: 벽(월드 좌표)에 고정 ---
                {
                    StuckDart sd;
                    sd.attachedToBoard = false;
                    sd.scored = false;
                    sd.worldPos = dartWorldPos;
                    sd.worldDir = glm::normalize(dartInitialVel);
                    sd.player = activeThrowPlayer;
                    g_StuckDarts.push_back(sd);
                }

                isResetting = true;

                // 바닥과 동일: 0점, 던진 횟수 증가
                if (activeThrowPlayer == 1) player1Throws++;
                else if (activeThrowPlayer == 2) player2Throws++;

                snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Wall...", activeThrowPlayer);
            }
            // ======================================
            // 3) 바닥 충돌
            // ======================================
            else if (dartWorldPos.y < -8.0f) {
                isFired = false;
                dartWorldPos.y = -8.0f;

                // --- 저장: 바닥에 고정(월드 좌표) ---
                {
                    StuckDart sd;
                    sd.attachedToBoard = false;
                    sd.scored = false;
                    sd.worldPos = dartWorldPos;
                    sd.worldDir = glm::normalize(dartInitialVel);
                    sd.player = activeThrowPlayer;
                    g_StuckDarts.push_back(sd);
                }

                isResetting = true;

                // 실격/바닥: 0점, 카운트만 증가
                if (activeThrowPlayer == 1) player1Throws++;
                else if (activeThrowPlayer == 2) player2Throws++;

                snprintf(scoreMessage, sizeof(scoreMessage), "Player %d Floor...", activeThrowPlayer);
            }
        }
        glutPostRedisplay();
        glutTimerFunc(16, ::timer, 0);
    }
}