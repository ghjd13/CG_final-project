#include "globals.h"
#include "texture_util.h" 

unsigned int shaderProgram = 0;
int windowWidth = 800;
int windowHeight = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;
bool isCameraMode = false;

bool isFired = false;
float dartTime = 0.0f;
glm::vec3 dartStartPos = glm::vec3(0.0f, -1.0f, 12.0f);
glm::vec3 dartWorldPos = dartStartPos;
glm::vec3 dartLaunchPos = dartStartPos;
glm::vec3 dartInitialVel = glm::vec3(0.0f);
glm::vec3 currentDartAim = glm::vec3(0.0f);

bool isResetting = false;
float resetTimer = 0.0f;

float lightSwingAngle = 0.0f;
glm::vec3 swingingLightPos = glm::vec3(0.0f, 4.0f, 5.0f);

Shader* g_Shader = nullptr;
Plane* g_Plane = nullptr;
ObjMesh* g_DartShape = nullptr;
Dartboard* g_Board = nullptr;
Cube* g_LightObj = nullptr;
Cube* g_Machine = nullptr; // optional

// 텍스처 ID 초기화
GLuint texFloor = 0, texFloorNormal = 0;
GLuint texWall = 0, texWallNormal = 0;
GLuint texDart = 0;
GLuint texDartBlue = 0;
GLuint texBoard = 0; // 다트판 컬러
GLuint texBoardNormal = 0; // 다트판 노말
GLuint texDefaultNormal = 0;
GLuint texMachine = 0;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
float boardRotAngle = 0.0f;
float machineRotAngle = 0.0f;

int currentPlayer = 1;
int activeThrowPlayer = 0;
int player1SetPoints = 0;
int player2SetPoints = 0;
int player1Throws = 0;
int player2Throws = 0;
int player1SetsWon = 0;
int player2SetsWon = 0;
int currentSetNumber = 1;
bool matchOver = false;

int gameScore = 0;
char scoreMessage[100] = "Ready";

// Stuck darts 저장소
std::vector<StuckDart> g_StuckDarts;

void clearStuckDarts() {
    g_StuckDarts.clear();
}

// 기존 init 함수 (텍스처/오브젝트 초기화)
void initGlobalObjects() {
    glEnable(GL_DEPTH_TEST);

    g_Shader = new Shader("vertex.glsl", "fragment.glsl");
    shaderProgram = g_Shader->ID;

    // 객체 생성
    g_Plane = new Plane(10.0f);

    // [경로 확인] resource 폴더 안의 파일명으로 설정 (한글->영어 변경 필수)
    g_DartShape = new ObjMesh("resource/dart.obj");

    g_Board = new Dartboard();
    g_LightObj = new Cube();
    g_Machine = new Cube();

    // 텍스처 로드
    texFloor = loadTexture("resource/uvMap.png");
    texFloorNormal = loadTexture("resource/normalMap.png");

    texWall = loadTexture("resource/wallUV.png");
    texWallNormal = loadTexture("resource/wallNormal.png");

    texDart = loadTexture("resource/dart.png"); // 1P 다트 텍스처
    texDartBlue = loadTexture("resource/dart_blue.png");
    if (texDartBlue == 0) texDartBlue = createColorTexture(0, 120, 255); // 폴백 파랑

    texBoard = createDartboardTexture(1024);
    texBoardNormal = createDartboardNormalTexture(512);

    texMachine = createColorTexture(30, 30, 30);

    texDefaultNormal = createFlatNormalTexture();
}