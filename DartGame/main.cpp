#include "globals.h"
#include "callbacks.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL Dart Game");

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Init Failed" << std::endl;
        return -1;
    }

    initGlobalObjects();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutPassiveMotionFunc(passiveMotion);
    glutMouseFunc(mouse);

    // [추가] 키보드 콜백 등록
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}