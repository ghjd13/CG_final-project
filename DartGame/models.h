#ifndef MODELS_H
#define MODELS_H

// --- 1. 필수 C++/C 헤더를 먼저 포함하여 기본 타입 정의 안정화 ---
#include <cstddef>      // std::size_t 정의를 위해
#include <stddef.h>     // offsetof 사용을 위해
#include <cmath>        // std::sin, std::cos 사용

// --- 2. OpenGL 및 GLM 헤더 ---
#include <gl/glew.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp> 
#include <vector>
#include <iostream>

// --- PI 상수 정의 ---
#ifndef PI_F
#define PI_F 3.141592653589793f
#endif

// --- 구조체 정의 ---
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color; // <<< 다트판 색상 구분을 위해 색상 정보를 추가했습니다.
};

// ModelData 구조체 정의
struct ModelData {
    GLuint vaoID;
    std::size_t vertexCount;
    glm::vec3 defaultColor; // 이 값은 조명 색상 외에 객체 색상으로 사용됨
};

// --- 모델 초기화 함수 선언 ---
ModelData SetupModel(const std::vector<Vertex>& vertices, const glm::vec3& color);

// --- 데이터 선언 ---

// 1. 다트 모델 데이터 (Vertex, Normal)
const std::vector<Vertex> DART_VERTICES = {
    // 다트 몸체는 단색 (빨강)으로 표시되므로 color는 1.0f, 0.0f, 0.0f로 설정
    {{ 0.0f,  0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}}, // Tip
    {{-0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}}, // Base Left
    {{ 0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}}, // Base Right
};

// 2. 다트판 모델 데이터 (24등분, 흰색/빨강색)
const std::vector<Vertex> TARGET_VERTICES = []() {
    std::vector<Vertex> vertices;
    const int segments = 24; // 24등분
    const float radius = 2.0f;
    const glm::vec3 normal = { 0.0f, 0.0f, 1.0f };
    const glm::vec3 colorWhite = { 1.0f, 1.0f, 1.0f }; // 흰색
    const glm::vec3 colorRed = { 1.0f, 0.0f, 0.0f };   // 빨강색

    // 중앙 정점 (모든 삼각형이 공유)
    vertices.push_back({ {0.0f, 0.0f, 0.0f}, normal, colorWhite }); // 중앙 색상은 흰색으로 임의 설정

    for (int i = 0; i < segments; ++i) {
        // 현재 세그먼트의 시작/끝 각도
        float angleStart = (float)i / segments * 2.0f * PI_F;
        float angleEnd = (float)(i + 1) / segments * 2.0f * PI_F;

        // 세그먼트 색상: 짝수 세그먼트는 흰색, 홀수 세그먼트는 빨강색
        const glm::vec3 segColor = (i % 2 == 0) ? colorWhite : colorRed;

        // 정점 1 (세그먼트 시작점)
        vertices.push_back({
            {radius * std::cos(angleStart), radius * std::sin(angleStart), 0.0f},
            normal,
            segColor
            });

        // 정점 2 (세그먼트 끝점)
        vertices.push_back({
            {radius * std::cos(angleEnd), radius * std::sin(angleEnd), 0.0f},
            normal,
            segColor
            });
    }
    // vertices.size()는 1 (중앙) + 24 * 2 (세그먼트) = 49 (Triangle Fan을 위한 정점 수)
    return vertices;
    }();

// 3. 광원 시각화 모델 (작은 구체)
const std::vector<Vertex> LIGHT_SPHERE_VERTICES = []() {
    std::vector<Vertex> vertices;
    const int rings = 12;
    const int sectors = 24;
    const float radius = 0.1f;
    const glm::vec3 color = { 1.0f, 1.0f, 0.0f }; // 광원은 노란색으로 시각화

    for (int i = 0; i <= rings; ++i) {
        for (int j = 0; j <= sectors; ++j) {
            float x = radius * std::sin(PI_F / rings * i) * std::cos(2 * PI_F / sectors * j);
            float y = radius * std::cos(PI_F / rings * i);
            float z = radius * std::sin(PI_F / rings * i) * std::sin(2 * PI_F / sectors * j);
            glm::vec3 pos(x, y, z);
            vertices.push_back({ pos, glm::normalize(pos), color });
        }
    }
    return vertices;
    }();

#endif // MODELS_H