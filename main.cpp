
#include "common.h"
#include "utils.h"

#include "application/Application.h"
#include "shader/Shader.h"

GLuint vao = 0;
Shader* shader = nullptr;

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

void prepareSingleBuffer() {
    float vtx[] = {
        -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
    };
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    GLuint pos_vbo = 0, color_vbo = 0;
    GL_CALL(glGenBuffers(1, &pos_vbo));
    GL_CALL(glGenBuffers(1, &color_vbo));
    //GLuint vbos[] = { 0, 0, 0 };
    //GL_CALL(glGenBuffers(3, vbos));

    vao = 0; // һ��vao�Ϳ��԰󶨶��vbo�����Զ�Ӧһ����mesh
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    /* Position buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pos_vbo)); // ����vbo֮����������������Ż����vbo���
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW));
	GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* Color buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color_vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* ������ɣ����Խ��һ�� */
	GL_CALL(glBindVertexArray(0)); // TODO:����д�ɺ�

    //GL_CALL(glDeleteBuffers(3, vbos));
    GL_CALL(glDeleteBuffers(1, &pos_vbo));
}


void prepareInterleavedBuffer() {
    float pos_color[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 0.4f, 0.5f, 0.6f,
		 0.8f,  0.8f, 0.0f, 0.2f, 0.3f, 0.4f,
		 0.8f,  0.0f, 0.0f, 0.7f, 0.8f, 0.9f
    };

	unsigned int index[] = {
		0, 1, 2,
		2, 3, 4,
		4, 5, 0
	};

    /* Create VBO */
    GLuint pos_color_vbo = 0; // interleaved vbo
    GL_CALL(glGenBuffers(1, &pos_color_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pos_color_vbo)); // bind vbo
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pos_color), pos_color, GL_STATIC_DRAW)); // send data

    /* Create EBO */
    GLuint ebo = 0;
    GL_CALL(glGenBuffers(1, &ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)); // bind ebo
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW)); // send data

    /* Create VAO */
    GL_CALL(glGenVertexArrays(1, &vao));

    /* Bind VBO & EBO with VAO */
	setVAO(vao, {
        /* Position vao */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0)); // connect current vao and vbo

        /* Color vao */
        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));

        /* ebo */
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)); // connect ebo and vao
	});

    //GL_CALL(glDeleteBuffers(1, &pos_color_vbo));
}


void prepareShader() {
	shader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
}

void render() {
    /* Clear canvas */
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    
    shader->begin();    // Bind User Shader
	GL_CALL(glBindVertexArray(vao)); // Bind Current VAO
    
    /* Draw */
    GL_CALL(glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0));
    //GL_CALL(glDrawArrays(GL_TRIANGLE_FAN, 0, 6));

	GL_CALL(glBindVertexArray(0));  // De-Bind Current VAO
	shader->end();      // De-Bind User Shader
}


int main() {
    bool status = APP->init(800, 600);
    if (!status) return -1;

    // set callback monitor
    APP->setResizeCallback(onResize);
    APP->setKeyboardCallback(onKeyChange);
   
    GL_CALL(glViewport(0, 0, 800, 600));
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    prepareInterleavedBuffer();
    prepareShader();

    // window loop
    while (APP->update()) {
        render();
    }

    APP->destroy();

    return 0;
}