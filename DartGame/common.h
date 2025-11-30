#ifndef COMMON_H
#define COMMON_H

// OpenGL 라이브러리 포함
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM 수학 라이브러리 포함
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <GL/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

// 정점 구조체 정의
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

#endif