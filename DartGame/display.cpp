#define _CRT_SECURE_NO_WARNINGS
#include "callbacks.h"
#include "globals.h"

// 램프 쉐이더 소스
static const char* lamp_vs_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* lamp_fs_source = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 lightColor;
void main() {
    FragColor = vec4(lightColor, 1.0); 
}
)";

static GLuint lampProgramID = 0;

static void initLampShader() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &lamp_vs_source, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &lamp_fs_source, NULL);
    glCompileShader(fs);
    lampProgramID = glCreateProgram();
    glAttachShader(lampProgramID, vs);
    glAttachShader(lampProgramID, fs);
    glLinkProgram(lampProgramID);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

static void renderText(float x, float y, void* font, const char* string) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glUseProgram(0);
    glRasterPos2f(x, y);
    for (const char* c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    g_Shader->use();
}

namespace Game {
    void display() {
        if (lampProgramID == 0) initLampShader();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_Shader->use();
        g_Shader->setVec3("lightPos", swingingLightPos);
        g_Shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        g_Shader->setVec3("viewPos", cameraPos);

        // 쉐이더 텍스처 슬롯 지정 (0번: 색상, 1번: 노말)
        g_Shader->setInt("texture_diffuse", 0);
        g_Shader->setInt("texture_normal", 1);

        // 기본 objectColor: 흰색 (원래 텍스처 유지)
        g_Shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
        g_Shader->setFloat("tintFactor", 0.0f);

        // 뷰, 프로젝션 행렬 설정
        glm::mat4 view;
        if (isCameraMode) view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        else view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        g_Shader->setMat4("view", view);
        g_Shader->setMat4("projection", projection);

        // ==========================================
        // 1. 바닥 (Floor) 그리기
        // ==========================================
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texFloor);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texFloorNormal);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f)); // 아래로 내림
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 눕히기
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 1.0f)); // 크기 40x40
        g_Shader->setMat4("model", model);
        g_Plane->draw();

        // ==========================================
        // 2. 벽 (Walls) 그리기 - 타일 텍스처
        // ==========================================
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texWall);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texWallNormal);

        // [뒷벽]
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, -20.0f));
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 1.0f)); // 가로 40, 높이 20
        g_Shader->setMat4("model", model);
        g_Plane->draw();

        // [왼쪽 벽]
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-20.0f, 10.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 1.0f));
        g_Shader->setMat4("model", model);
        g_Plane->draw();

        // [오른쪽 벽]
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, 10.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 1.0f));
        g_Shader->setMat4("model", model);
        g_Plane->draw();

        // ==========================================
        // 3. 다트판 (기본 텍스처 + 평면 노말)
        // ==========================================
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texBoard);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texBoardNormal);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
        model = glm::rotate(model, glm::radians(boardRotAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 1.0f));
        g_Shader->setMat4("model", model);
        g_Board->draw();

        // ==========================================
        // 저장된(붙어있는) 다트들 렌더
        // - scored == true 다트만 보드 로컬 좌표로 변환해 보드와 함께 회전
        // - scored == false 다트는 월드 좌표로 고정 렌더
        // ==========================================
        for (const StuckDart& sd : g_StuckDarts) {
            g_Shader->use();

            // 플레이어별 텍스처
            GLuint dartTex = (sd.player == 2) ? texDartBlue : texDart;
            glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, dartTex);
            glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texDefaultNormal);

            glm::mat4 m(1.0f);

            if (sd.scored && sd.attachedToBoard) {
                // scored인 경우: localPos/localDir 사용 -> 보드 모델 곱해 회전 유지
                // 변경: 보드의 scale이 다트 모델에 영향을 주지 않도록, 먼저 보드 transform (translate+rotate+scale)로 world position 계산하고
                // 다트는 world 위치에 놓은 다음 오직 회전(보드 회전에 따른 방향)만 적용해서 동일한 최종 스케일(0.2f)을 유지합니다.

                glm::mat4 boardModel = glm::mat4(1.0f);
                boardModel = glm::translate(boardModel, glm::vec3(0.0f, 0.0f, -10.0f));
                boardModel = glm::rotate(boardModel, glm::radians(boardRotAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                boardModel = glm::scale(boardModel, glm::vec3(5.0f, 5.0f, 1.0f));

                // 약간 표면 앞쪽으로 오프셋 (로컬 Z)
                const float offset = 0.02f;
                glm::vec3 localWithOffset = sd.localPos + glm::vec3(0.0f, 0.0f, offset);

                // 보드 모델(스케일 포함)을 사용해 worldPos 계산 (이 값만 사용)
                glm::vec3 worldPos = glm::vec3(boardModel * glm::vec4(localWithOffset, 1.0f));

                // 모델은 worldPos 기준으로 구성 -> 보드 스케일이 다트 크기에 곱해지지 않음
                m = glm::translate(glm::mat4(1.0f), worldPos);

                // localDir -> worldDir 변환 (보드 회전만)
                glm::mat3 boardRotMatCurr = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(boardRotAngle), glm::vec3(0,0,1)));
                glm::vec3 worldDir = boardRotMatCurr * sd.localDir;
                if (glm::length(worldDir) < 1e-6f) worldDir = glm::vec3(0.0f, 0.0f, -1.0f);
                else worldDir = glm::normalize(worldDir);

                float pitch = glm::degrees(asin(glm::clamp(worldDir.y, -1.0f, 1.0f)));
                float yaw = glm::degrees(atan2(worldDir.x, -worldDir.z));

                m = glm::rotate(m, glm::radians(-yaw), glm::vec3(0, 1, 0));
                m = glm::rotate(m, glm::radians(pitch), glm::vec3(1, 0, 0));
                m = glm::rotate(m, glm::radians(180.0f), glm::vec3(1, 0, 0));
            } else {
                // scored가 아니면 월드 위치/방향 그대로 고정 렌더
                m = glm::translate(m, sd.worldPos);

                glm::vec3 wd = sd.worldDir;
                if (glm::length(wd) < 1e-6f) wd = glm::vec3(0.0f, 0.0f, -1.0f);
                else wd = glm::normalize(wd);

                float pitch = glm::degrees(asin(glm::clamp(wd.y, -1.0f, 1.0f)));
                float yaw = glm::degrees(atan2(wd.x, -wd.z));

                m = glm::rotate(m, glm::radians(-yaw), glm::vec3(0, 1, 0));
                m = glm::rotate(m, glm::radians(pitch), glm::vec3(1, 0, 0));
                m = glm::rotate(m, glm::radians(180.0f), glm::vec3(1, 0, 0));
            }

            // 공통: 항상 동일한 최종 스케일 적용(비행 중 다트와 일치하도록)
            const glm::vec3 dartScale(0.2f, 0.2f, 0.2f);
            m = glm::scale(m, dartScale);
            g_Shader->setMat4("model", m);
            g_DartShape->draw();
        }

        // ==========================================
        // 4. 다트 (현재 날아가는 다트) - 현재 isFired일 때만 그림,
        //    조준(프리뷰)도 표시
        // ==========================================
        if (isFired || (!isFired && !isResetting && !matchOver)) {
            g_Shader->use(); // 쉐이더 활성화

            // 텍스처 슬롯/노말 다시 설정
            g_Shader->setInt("texture_diffuse", 0);
            g_Shader->setInt("texture_normal", 1);

            // 프리뷰인지 실제 발사인지 판단
            bool preview = !isFired;
            int displayPlayer = preview ? currentPlayer : activeThrowPlayer;

            // 다트 텍스처과 Tint 설정 (tintFactor 사용)
            if (displayPlayer == 2) {
                g_Shader->setVec3("objectColor", glm::vec3(0.0f, 0.6f, 1.0f));
                g_Shader->setFloat("tintFactor", 1.0f);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texDartBlue ? texDartBlue : texDart);
            } else {
                g_Shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
                g_Shader->setFloat("tintFactor", 0.0f);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texDart);
            }

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texDefaultNormal);

            // 위치/각도 결정
            glm::mat4 m = glm::mat4(1.0f);
            glm::vec3 pos = preview ? dartStartPos : dartWorldPos; // 발사 전 프리뷰는 시작 위치
            m = glm::translate(m, pos);

            float pitch = currentDartAim.x;
            float yaw = currentDartAim.y;
            if (!preview && isFired) pitch -= dartTime * 15.0f; // 날아가는 경우만 시간에 따른 휘어짐 적용

            m = glm::rotate(m, glm::radians(-yaw), glm::vec3(0, 1, 0));
            m = glm::rotate(m, glm::radians(pitch), glm::vec3(1, 0, 0));
            m = glm::rotate(m, glm::radians(180.0f), glm::vec3(1, 0, 0)); // OBJ 방향에 따라 조절

            // 비행 중 다트와 stuck 다트의 크기 동일하게 유지 (동일 dartScale 사용)
            const glm::vec3 dartScale(0.2f, 0.2f, 0.2f);
            m = glm::scale(m, dartScale);
            g_Shader->setMat4("model", m);
            g_DartShape->draw();

            // 다트 렌더 이후 tint 복구(다른 오브젝트 영향 방지)
            g_Shader->setFloat("tintFactor", 0.0f);
            g_Shader->setVec3("objectColor", glm::vec3(1.0f,1.0f,1.0f));
        }

        // 램프
        glUseProgram(lampProgramID);
        glUniformMatrix4fv(glGetUniformLocation(lampProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lampProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(lampProgramID, "lightColor"), 1.0f, 1.0f, 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, swingingLightPos);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lampProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        g_LightObj->draw();

        g_Shader->use();
        glColor3f(1.0f, 1.0f, 1.0f);
        char scoreBuf[50];
        snprintf(scoreBuf, sizeof(scoreBuf), "Score: %d", gameScore);
        renderText(-0.95f, 0.9f, GLUT_BITMAP_TIMES_ROMAN_24, scoreBuf);

        if (isCameraMode)
            renderText(-0.95f, 0.8f, GLUT_BITMAP_HELVETICA_18, "[Camera Mode] WASD: Move, Mouse: Look, C: Game Mode");
        else {
            renderText(-0.3f, -0.8f, GLUT_BITMAP_HELVETICA_18, scoreMessage);
            renderText(-0.95f, 0.8f, GLUT_BITMAP_HELVETICA_12, "[Game Mode] Click: Shoot, C: Camera Mode");
        }

        // UI: 세트 점수/현재 세트/현재 플레이어/메시지
        g_Shader->use();
        glColor3f(1.0f, 1.0f, 1.0f);

        char buf[256];
        snprintf(buf, sizeof(buf), "Set %d / P1 Sets: %d  P2 Sets: %d", currentSetNumber, player1SetsWon, player2SetsWon);
        renderText(-0.95f, 0.95f, GLUT_BITMAP_HELVETICA_18, buf);

        snprintf(buf, sizeof(buf), "SetScore  P1: %d (%d throws)   P2: %d (%d throws)", player1SetPoints, player1Throws, player2SetPoints, player2Throws);
        renderText(-0.95f, 0.90f, GLUT_BITMAP_HELVETICA_12, buf);

        if (!matchOver) {
            snprintf(buf, sizeof(buf), "Current Turn: Player %d", currentPlayer);
            renderText(-0.95f, 0.85f, GLUT_BITMAP_HELVETICA_12, buf);
        } else {
            renderText(-0.5f, 0.0f, GLUT_BITMAP_HELVETICA_18, scoreMessage);
        }

        // 상태 메시지 (가장 중요)
        renderText(-0.95f, -0.85f, GLUT_BITMAP_HELVETICA_12, scoreMessage);

        glutSwapBuffers();
    }
}