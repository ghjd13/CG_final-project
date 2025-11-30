#ifndef GLOBALS_H
#define GLOBALS_H

#include "common.h"
#include "shader_util.h"
#include "objects.h" 

extern unsigned int shaderProgram;
extern int windowWidth;
extern int windowHeight;

// 카메라
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern float cameraYaw;
extern float cameraPitch;
extern bool isCameraMode;

// 다트 상태
extern bool isFired;
extern float dartTime;
extern glm::vec3 dartStartPos;
extern glm::vec3 dartWorldPos;
extern glm::vec3 dartLaunchPos;
extern glm::vec3 dartInitialVel;
extern glm::vec3 currentDartAim;

extern bool isResetting;
extern float resetTimer;

// 조명
extern float lightSwingAngle;
extern glm::vec3 swingingLightPos;

// 객체 포인터
extern Shader* g_Shader;
extern Plane* g_Plane;
extern ObjMesh* g_DartShape;
extern Dartboard* g_Board;
extern Cube* g_LightObj;
extern Cube* g_Machine; // optional housing

// 텍스처 ID
extern GLuint texFloor, texFloorNormal;
extern GLuint texWall, texWallNormal;
extern GLuint texDart;      // 1P 다트
extern GLuint texDartBlue;  // 2P 다트 (파란색)
extern GLuint texBoard;         // 다트판 컬러 텍스처
extern GLuint texBoardNormal;   // 다트판 전용 노말 텍스처 (있다면)
extern GLuint texDefaultNormal; // 기본 노말 맵
extern GLuint texMachine;       // optional

// 시간/회전
extern float lastFrameTime;
extern float deltaTime;
extern float boardRotAngle;
extern float machineRotAngle;

// 멀티플레이어 상태
extern int currentPlayer;         // 1 또는 2 (현재 차례)
extern int activeThrowPlayer;     // 날아가는 다트를 던진 플레이어 (0 = 없음)
extern int player1SetPoints;      // 현재 세트 내 누적 점수 (1P)
extern int player2SetPoints;      // 현재 세트 내 누적 점수 (2P)
extern int player1Throws;         // 현재 세트에서 1P가 던진 횟수 (<=5)
extern int player2Throws;         // 현재 세트에서 2P가 던진 횟수 (<=5)
extern int player1SetsWon;        // 세트 승수
extern int player2SetsWon;        // 세트 승수
extern int currentSetNumber;      // 1..5
extern bool matchOver;            // 매치가 끝났는가

extern int gameScore; // 기존 용도 유지
extern char scoreMessage[100];

// ========== Stuck darts (세트 동안 남아있는 다트) ==========
// 변경: 충돌 시점의 worldPos/worldDir 저장 + 보드 로컬 좌표(localPos/localDir)
// scored == true 인 경우(점수 획득)은 보드 로컬 좌표로 저장하여 보드와 함께 회전
// scored == false 인 경우(보드 외부/벽/바닥)는 월드 위치로 고정 렌더
struct StuckDart {
    bool attachedToBoard;    // true: 보드에 닿았음(정보 용도), false: 땅/벽
    glm::vec3 worldPos;      // 충돌 시점 월드 좌표 (렌더할 때 사용)
    glm::vec3 worldDir;      // 충돌 시점 월드 방향 (orientation)
    glm::vec3 localPos;      // 보드 로컬 좌표 (scored == true 인 경우 사용)
    glm::vec3 localDir;      // 보드 로컬 방향 (scored == true 인 경우 사용)
    bool scored;             // true: 점수 획득(보드 영역), 보드와 함께 회전시킬 항목
    int player;              // 1 또는 2 (다트 색상/텍스처 결정)
};

extern std::vector<StuckDart> g_StuckDarts;
void clearStuckDarts();

void initGlobalObjects();

#endif