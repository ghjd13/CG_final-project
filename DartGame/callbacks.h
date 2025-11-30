#ifndef CALLBACKS_H
#define CALLBACKS_H

// GLUT 콜백 함수들
void display();
void reshape(int w, int h);
void timer(int value);
void passiveMotion(int x, int y);
void mouse(int button, int state, int x, int y);
// [추가] 키보드 입력
void keyboard(unsigned char key, int x, int y);

namespace Game {
    void display();
    void reshape(int w, int h);
    void timer(int value);
    void passiveMotion(int x, int y);
    void mouse(int button, int state, int x, int y);
    void keyboard(unsigned char key, int x, int y);
}

#endif