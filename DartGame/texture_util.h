#ifndef TEXTURE_UTIL_H
#define TEXTURE_UTIL_H

#include "common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <cmath>
#include <algorithm>

// [New] 파일 경로에서 텍스처 로드하는 함수
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // OpenGL은 Y축 좌표가 반대이므로 이미지를 뒤집어서 로드
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;
        else format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 텍스처 반복 설정 (타일링을 위해 필수)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // 필터링 설정 (축소시 선형 보간 + 밉맵, 확대시 선형 보간)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded: " << path << std::endl;
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// [New] 노말 맵이 없는 물체를 위한 '평평한 노말 맵' (Flat Blue) 생성
// 다트(OBJ)에는 노말 맵이 없지만 쉐이더는 노말 맵을 요구하므로 이걸 사용합니다.
GLuint createFlatNormalTexture() {
    GLubyte data[1][1][3];
    data[0][0][0] = 128; // R = 0.5 (X축 기울기 0)
    data[0][0][1] = 128; // G = 0.5 (Y축 기울기 0)
    data[0][0][2] = 255; // B = 1.0 (Z축 수직)

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    return textureID;
}

// 간단한 체크무늬 텍스처 생성기 (이미지 파일 없이 테스트 가능하도록)
GLuint createCheckerTexture() {
    const int width = 64;
    const int height = 64;
    GLubyte data[height][width][3]; // RGB

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // x, y 좌표에 따라 검은색/흰색 교차
            int c = ((((y & 8) == 0) ^ ((x & 8)) == 0)) * 255;
            data[y][x][0] = (GLubyte)c; // R
            data[y][x][1] = (GLubyte)c; // G
            data[y][x][2] = (GLubyte)c; // B
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 텍스처 파라미터 설정 (반복, 선형 필터링)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 이미지 데이터 전송
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
}

// 색상 텍스처 생성기 (단색 텍스처가 필요할 때)
GLuint createColorTexture(GLubyte r, GLubyte g, GLubyte b) {
    GLubyte data[1][1][3];
    data[0][0][0] = r;
    data[0][0][1] = g;
    data[0][0][2] = b;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    return textureID;
}

// 프로그래매틱 다트판 생성 (권장 size = 512 또는 1024)
GLuint createDartboardTexture(int size = 1024) {
    const int S = std::max(64, size);
    std::vector<GLubyte> img(S * S * 3);
    const float cx = S * 0.5f;
    const float cy = S * 0.5f;
    const float boardR = S * 0.45f; // 판 끝 반지름 (픽셀)

    // 표준 다트 섹션 순서 (시계방향, 20이 위쪽(정북) 기준)
    const int sectorOrder[20] = {20,1,18,4,13,6,10,15,2,17,3,19,7,16,8,11,14,9,12,5};

    // 비율: 전체 반지름을 1로 정규화한 링 위치
    const float innerBullR = 0.05f;
    const float outerBullR = 0.12f;
    const float tripleMinR = 0.52f;
    const float tripleMaxR = 0.60f;
    const float doubleMinR = 0.78f;
    const float doubleMaxR = 0.90f;

    for (int y = 0; y < S; ++y) {
        for (int x = 0; x < S; ++x) {
            float dx = x + 0.5f - cx;
            float dy = cy - (y + 0.5f); // 이미지 y 방향 보정
            float dist = std::sqrt(dx*dx + dy*dy);
            float nr = dist / boardR; // 0..1 이상
            float ang = std::atan2(dy, dx); // -pi..pi
            float deg = ang * 180.0f / 3.14159265f;
            if (deg < 0) deg += 360.0f;

            // 기본 배경(외부) : 나무색 또는 어두운 색
            GLubyte r = 60, g = 40, b = 20;

            if (nr <= 1.0f) {
                // 내부 (보드 영역)
                if (nr <= innerBullR) {
                    // inner bull : 골드
                    r = 220; g = 170; b = 30;
                }
                else if (nr <= outerBullR) {
                    // outer bull : 레드
                    r = 200; g = 0; b = 0;
                }
                else {
                    // 섹션 번호 계산 (0..19)
                    int sector = (int)(deg / (360.0f / 20.0f)) % 20;
                    bool isEvenSector = ((sector % 2) == 0);

                    // 기본 싱글 색 (검정/크림색 번갈아) ? 실제 판은 검정/아이보리 느낌
                    GLubyte singleDarkR = 20, singleDarkG = 20, singleDarkB = 20;
                    GLubyte singleLightR = 230, singleLightG = 220, singleLightB = 180;

                    // 링 색 (레드 / 그린)
                    GLubyte ringRedR = 210, ringRedG = 20, ringRedB = 20;
                    GLubyte ringGreenR = 20, ringGreenG = 110, ringGreenB = 30;

                    // 트리플/더블 링 처리
                    if (nr >= tripleMinR && nr <= tripleMaxR) {
                        if (isEvenSector) { r = ringGreenR; g = ringGreenG; b = ringGreenB; }
                        else               { r = ringRedR;   g = ringRedG;   b = ringRedB;   }
                    }
                    else if (nr >= doubleMinR && nr <= doubleMaxR) {
                        if (isEvenSector) { r = ringGreenR; g = ringGreenG; b = ringGreenB; }
                        else               { r = ringRedR;   g = ringRedG;   b = ringRedB;   }
                    }
                    else {
                        // 싱글 영역: 원형 섹션마다 밝기 교차
                        if (isEvenSector) { r = singleDarkR; g = singleDarkG; b = singleDarkB; }
                        else              { r = singleLightR; g = singleLightG; b = singleLightB; }
                    }
                }
            }

            int idx = (y * S + x) * 3;
            img[idx + 0] = r;
            img[idx + 1] = g;
            img[idx + 2] = b;
        }
    }

    // 텍스처 업로드
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, S, S, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    return texID;
}

// 간단한 노말 맵 생성: 링/불즈 주변에 약한 높이(범프)를 생성하고 중앙차분으로 노말 계산
GLuint createDartboardNormalTexture(int size = 512) {
    const int S = std::max(64, size);
    std::vector<float> height(S * S, 0.0f);
    const float cx = S * 0.5f;
    const float cy = S * 0.5f;
    const float boardR = S * 0.45f;

    const float innerBullR = 0.05f;
    const float outerBullR = 0.12f;
    const float tripleMinR = 0.52f;
    const float tripleMaxR = 0.60f;
    const float doubleMinR = 0.78f;
    const float doubleMaxR = 0.90f;

    // 높이 필드 생성: 링과 불즈에 작은 bump
    for (int y = 0; y < S; ++y) {
        for (int x = 0; x < S; ++x) {
            float dx = x + 0.5f - cx;
            float dy = cy - (y + 0.5f);
            float dist = std::sqrt(dx*dx + dy*dy);
            float nr = dist / boardR;

            float h = 0.0f;
            // bull 약간 솟음
            if (nr <= outerBullR) h += 0.08f * (1.0f - nr / outerBullR);
            // triple ring 돌출
            float tcenter = (tripleMinR + tripleMaxR) * 0.5f;
            float twidth = (tripleMaxR - tripleMinR) * 0.5f;
            h += 0.06f * std::exp(- (nr - tcenter)*(nr - tcenter) / (twidth * twidth * 4.0f));
            // double ring 돌출
            float dcenter = (doubleMinR + doubleMaxR) * 0.5f;
            float dwidth = (doubleMaxR - doubleMinR) * 0.5f;
            h += 0.05f * std::exp(- (nr - dcenter)*(nr - dcenter) / (dwidth * dwidth * 4.0f));

            height[y * S + x] = h;
        }
    }

    // 높이에서 노말로 변환 (중앙차분)
    std::vector<GLubyte> normalImg(S * S * 3);
    const float strength = 1.0f; // 높이->노말 영향 크기
    for (int y = 0; y < S; ++y) {
        for (int x = 0; x < S; ++x) {
            auto sample = [&](int sx, int sy) {
                sx = std::min(std::max(sx, 0), S-1);
                sy = std::min(std::max(sy, 0), S-1);
                return height[sy * S + sx];
            };
            float hl = sample(x-1, y);
            float hr = sample(x+1, y);
            float hd = sample(x, y-1);
            float hu = sample(x, y+1);

            float dx = (hr - hl) * strength;
            float dy = (hu - hd) * strength;
            glm::vec3 n = glm::normalize(glm::vec3(-dx, -dy, 1.0f));
            GLubyte nx = (GLubyte)( (n.x * 0.5f + 0.5f) * 255.0f );
            GLubyte ny = (GLubyte)( (n.y * 0.5f + 0.5f) * 255.0f );
            GLubyte nz = (GLubyte)( (n.z * 0.5f + 0.5f) * 255.0f );

            int idx = (y * S + x) * 3;
            normalImg[idx + 0] = nx;
            normalImg[idx + 1] = ny;
            normalImg[idx + 2] = nz;
        }
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, S, S, 0, GL_RGB, GL_UNSIGNED_BYTE, normalImg.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    return texID;
}

#endif