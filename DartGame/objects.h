#ifndef OBJECTS_H
#define OBJECTS_H

#include "common.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <gl/glew.h>
#include <gl/glm/glm.hpp>
#include <stddef.h> 

// 기본 메쉬 클래스
class Mesh {
public:
    GLuint VAO, VBO;
    std::vector<Vertex> vertices;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        GLsizei stride = sizeof(Vertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Tangent));

        glBindVertexArray(0);
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);
    }
};

// OBJ 파일 로더 클래스 (수정됨: 사각형 지원)
class ObjMesh : public Mesh {
public:
    ObjMesh(const char* filepath) {
        loadObj(filepath);
        setupMesh();
    }

private:
    void loadObj(const char* filepath) {
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to load OBJ: " << filepath << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                glm::vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                temp_positions.push_back(pos);
            }
            else if (prefix == "vt") {
                glm::vec2 uv;
                ss >> uv.x >> uv.y;
                temp_uvs.push_back(uv);
            }
            else if (prefix == "vn") {
                glm::vec3 norm;
                ss >> norm.x >> norm.y >> norm.z;
                temp_normals.push_back(norm);
            }
            else if (prefix == "f") {
                // 한 줄에 있는 모든 정점 데이터를 임시 저장
                std::vector<Vertex> faceVertices;
                std::string vertexStr;

                while (ss >> vertexStr) {
                    unsigned int posIdx = 0, uvIdx = 0, normIdx = 0;
                    size_t s1 = vertexStr.find('/');
                    size_t s2 = vertexStr.find('/', s1 + 1);

                    if (s1 == std::string::npos) {
                        posIdx = std::stoi(vertexStr);
                    }
                    else if (s2 == std::string::npos) {
                        posIdx = std::stoi(vertexStr.substr(0, s1));
                        uvIdx = std::stoi(vertexStr.substr(s1 + 1));
                    }
                    else {
                        posIdx = std::stoi(vertexStr.substr(0, s1));
                        if (s2 > s1 + 1) uvIdx = std::stoi(vertexStr.substr(s1 + 1, s2 - s1 - 1));
                        normIdx = std::stoi(vertexStr.substr(s2 + 1));
                    }

                    Vertex v;
                    v.Position = temp_positions[posIdx - 1];

                    if (uvIdx > 0 && uvIdx <= temp_uvs.size()) v.TexCoords = temp_uvs[uvIdx - 1];
                    else v.TexCoords = glm::vec2(0.0f);

                    if (normIdx > 0 && normIdx <= temp_normals.size()) v.Normal = temp_normals[normIdx - 1];
                    else v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

                    v.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                    faceVertices.push_back(v);
                }

                // [핵심 수정] 사각형(점 4개)을 삼각형 2개로 쪼개기
                if (faceVertices.size() >= 3) {
                    // 첫 번째 삼각형 (0, 1, 2)
                    vertices.push_back(faceVertices[0]);
                    vertices.push_back(faceVertices[1]);
                    vertices.push_back(faceVertices[2]);

                    // 만약 점이 4개라면? 두 번째 삼각형 추가 (0, 2, 3)
                    if (faceVertices.size() >= 4) {
                        vertices.push_back(faceVertices[0]);
                        vertices.push_back(faceVertices[2]);
                        vertices.push_back(faceVertices[3]);
                    }
                }
            }
        }
        std::cout << "Loaded OBJ: " << filepath << " (Vertices: " << vertices.size() << ")" << std::endl;
    }
};

// 단순 평면 클래스
class Plane : public Mesh {
public:
    Plane(float tileCount = 1.0f) {
        generateQuad(tileCount);
        setupMesh();
    }
private:
    void generateQuad(float tileCount) {
        glm::vec3 p1(-0.5f, -0.5f, 0.0f);
        glm::vec3 p2(0.5f, -0.5f, 0.0f);
        glm::vec3 p3(0.5f, 0.5f, 0.0f);
        glm::vec3 p4(-0.5f, 0.5f, 0.0f);
        glm::vec3 n(0.0f, 0.0f, 1.0f);
        glm::vec2 uv1(0.0f, 0.0f);
        glm::vec2 uv2(tileCount, 0.0f);
        glm::vec2 uv3(tileCount, tileCount);
        glm::vec2 uv4(0.0f, tileCount);
        glm::vec3 tangent(1.0f, 0.0f, 0.0f);

        vertices.push_back({ p1, n, uv1, tangent });
        vertices.push_back({ p2, n, uv2, tangent });
        vertices.push_back({ p3, n, uv3, tangent });
        vertices.push_back({ p1, n, uv1, tangent });
        vertices.push_back({ p3, n, uv3, tangent });
        vertices.push_back({ p4, n, uv4, tangent });
    }
};

class Dartboard : public Mesh {
public:
    Dartboard() {
        generateDisk(0.5f, 40);
        setupMesh();
    }
private:
    void generateDisk(float radius, int segments) {
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        glm::vec3 normal(0.0f, 0.0f, 1.0f);
        glm::vec3 tangent(1.0f, 0.0f, 0.0f);

        for (int i = 0; i < segments; i++) {
            float t1 = (float)i / segments * 2.0f * 3.14159f;
            float t2 = (float)(i + 1) / segments * 2.0f * 3.14159f;
            glm::vec3 p1(cos(t1) * radius, sin(t1) * radius, 0.0f);
            glm::vec3 p2(cos(t2) * radius, sin(t2) * radius, 0.0f);
            glm::vec2 uvCenter(0.5f, 0.5f);
            glm::vec2 uv1 = uvCenter + glm::vec2(cos(t1), sin(t1)) * 0.5f;
            glm::vec2 uv2 = uvCenter + glm::vec2(cos(t2), sin(t2)) * 0.5f;

            vertices.push_back({ center, normal, uvCenter, tangent });
            vertices.push_back({ p1, normal, uv1, tangent });
            vertices.push_back({ p2, normal, uv2, tangent });

            glm::vec3 n2(0, 0, -1);
            vertices.push_back({ center, n2, uvCenter, tangent });
            vertices.push_back({ p2, n2, uv2, tangent });
            vertices.push_back({ p1, n2, uv1, tangent });
        }
    }
};

// 큐브 데이터 (조명용)
class Cube : public Mesh {
public:
    Cube() { generateCube(0.5f); setupMesh(); }
private:
    void generateCube(float s) {
        glm::vec3 p1(-s, -s, s), p2(s, -s, s), p3(s, s, s), p4(-s, s, s);
        glm::vec3 p5(-s, -s, -s), p6(s, -s, -s), p7(s, s, -s), p8(-s, s, -s);
        addQuad(p1, p2, p3, p4, glm::vec3(0, 0, 1));
        addQuad(p6, p5, p8, p7, glm::vec3(0, 0, -1));
        addQuad(p4, p3, p7, p8, glm::vec3(0, 1, 0));
        addQuad(p5, p6, p2, p1, glm::vec3(0, -1, 0));
        addQuad(p2, p6, p7, p3, glm::vec3(1, 0, 0));
        addQuad(p5, p1, p4, p8, glm::vec3(-1, 0, 0));
    }
    void addQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 n) {
        glm::vec2 uv(0.0f); glm::vec3 t(1.0f, 0.0f, 0.0f);
        vertices.push_back({ p1, n, uv, t }); vertices.push_back({ p2, n, uv, t }); vertices.push_back({ p3, n, uv, t });
        vertices.push_back({ p1, n, uv, t }); vertices.push_back({ p3, n, uv, t }); vertices.push_back({ p4, n, uv, t });
    }
};

#endif