#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils.h"

#include "application/Application.h"


void onResize(int width, int height) {
    std::cout << "onResize" << std::endl;
    GL_CALL(glViewport(0, 0, width, height));
}

void onKeyChange(int key, int scancode, int action, int mods) {
    std::cout << (action == GLFW_RELEASE ? "release key: " :
        action == GLFW_PRESS ? "press a key: " :
        action == 2 ? "pressing key: " : "unknown action: ") << key << std::endl;
    std::cout << "action: " << action << std::endl;
    std::cout << "mods: " << mods << std::endl;
}


int main() {
    bool status = APP->init(800, 600);
    if (!status) return -1;

    // set callback monitor
    APP->setResizeCallback(onResize);
    APP->setKeyboardCallback(onKeyChange);
   
    GL_CALL(glViewport(0, 0, 800, 600));
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    // window loop
    while (APP->update()) {
        /* Clear canvas */
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        /* Rendering */

    }

    APP->destroy();

    return 0;
}