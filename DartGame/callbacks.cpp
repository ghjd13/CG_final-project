#define _CRT_SECURE_NO_WARNINGS
#include "callbacks.h"   // 본인 헤더

// ========================================================
// GLUT 콜백 함수들 (단순 연결 역할)
// ========================================================

void display() {
    Game::display();
}

void reshape(int w, int h) {
    Game::reshape(w, h);
}

void timer(int value) {
    Game::timer(value);
}

void passiveMotion(int x, int y) {
    Game::passiveMotion(x, y);
}

void mouse(int button, int state, int x, int y) {
    Game::mouse(button, state, x, y);
}

void keyboard(unsigned char key, int x, int y) {
    Game::keyboard(key, x, y);
}