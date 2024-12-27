
#include <string>

#include "common.h"
#include "utils.h"

#include "application/Application.h"
#include "shader/Shader.h"
#include "camera/Camera.h"


/* Application settings */
GLuint vao = 0;
Shader* shader = nullptr;
float viewport_width = 800.0f, viewport_height = 600.0f;

/* Time */
float last_frame = 0.0f;
float delta_time = 0.0f;

/* Camera */
Camera* camera = nullptr;
bool first_mouse = true;
float last_x = viewport_width / 2.0f, last_y = viewport_height / 2.0f;

void onResize(int width, int height) {
    std::cout << "onResize" << std::endl;
    GL_CALL(glViewport(0, 0, width, height));
}

void onKeyChange(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W:
            camera->processKeyboard(FORWARD, delta_time);
            break;
        case GLFW_KEY_S:
            camera->processKeyboard(BACKWARD, delta_time);
            break;
        case GLFW_KEY_A:
            camera->processKeyboard(LEFT, delta_time);
            break;
        case GLFW_KEY_D:
            camera->processKeyboard(RIGHT, delta_time);
            break;
        //  case GLFW_KEY_ESCAPE:
        //	    APP->destroy();
        //	    break;
        default:
            break;
        }
    }
}

void onMouseMove(double xpos_in, double ypos_in) {
    float xpos = static_cast<float>(xpos_in);
    float ypos = static_cast<float>(ypos_in);
    

    // first in, then do not react
	if (first_mouse) { 
        last_x = xpos; last_y = ypos;
        first_mouse = false;
		return;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; // reversed, because y-axis goes from bottom to top
    last_x = xpos; last_y = ypos;
    
	camera->processMouseMovement(xoffset, yoffset, true);
}

// not in use
void onMouseScroll(/* double xoffset, */double yoffset) {
	camera->processMouseScroll(yoffset);
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

    vao = 0; // 一个vao就可以绑定多个vbo，可以对应一整个mesh
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    /* Position buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pos_vbo)); // 绑定了vbo之后，下面的属性描述才会与此vbo相关
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW));
	GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* Color buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color_vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* 操作完成，可以解绑一下 */
	GL_CALL(glBindVertexArray(0)); // TODO:可以写成宏

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

void prepareCubeBuffer() {
    float cube_vertice[] = {
        -0.5f, -0.5f, -0.5f, /* 0: - - - */
         0.5f, -0.5f, -0.5f, /* 1: + - - */
        -0.5f,  0.5f, -0.5f, /* 2: - + - */
         0.5f,  0.5f, -0.5f, /* 3: + + - */
        -0.5f, -0.5f,  0.5f, /* 4: - - + */
         0.5f, -0.5f,  0.5f, /* 5: + - + */
        -0.5f,  0.5f,  0.5f, /* 6: - + + */
         0.5f,  0.5f,  0.5f, /* 7: + + + */
    };
    unsigned int cube_index[] = {
        4, 5, 6,
        6, 5, 7,
        5, 1, 7,
        7, 1, 3,
        1, 0, 3,
        3, 0, 2,
        0, 4, 2,
        2, 4, 6,
        6, 7, 2,
        2, 7, 3,
        5, 4, 1,
        1, 4, 0
    };

    /* Create VBO */
    GLuint cube_vbo = 0; // interleaved vbo
    GL_CALL(glGenBuffers(1, &cube_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo)); // bind vbo
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertice), cube_vertice, GL_STATIC_DRAW)); // send data

    /* Create EBO */
    GLuint cube_ebo = 0;
    GL_CALL(glGenBuffers(1, &cube_ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo)); // bind ebo
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW)); // send data

    /* Create VAO */
    GL_CALL(glGenVertexArrays(1, &vao));

    /* Bind VBO & EBO with VAO */
    setVAO(vao, {
        /* Position vao */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0)); // connect current vao with vbo

        /* ebo */
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo)); // connect current vao with ebo
    });
}

glm::vec3 object_positon[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f)
        //glm::vec3(-3.8f, -2.0f, -12.3f),
        //glm::vec3(2.4f, -0.4f, -3.5f),
        //glm::vec3(-1.7f,  3.0f, -7.5f),
        //glm::vec3(1.3f, -2.0f, -2.5f),
        //glm::vec3(1.5f,  2.0f, -2.5f),
        //glm::vec3(1.5f,  0.2f, -1.5f),
        //glm::vec3(-1.3f,  1.0f, -1.5f)
};

void prepareShader() {
	shader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
}


glm::mat4 model, view, projection;
void render() {
    /* Calculate time */
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    /* Clear canvas */
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    /* Set User Shader */
    shader->begin();    // Bind User Shader
	shader->setFloat("time", current_frame);

    /* set MVP */
    model = glm::rotate(model, current_frame, glm::vec3(0.5f, 1.0f, 0.0f));
    view = camera->getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    //projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 10.0f);

	//shader->setMat4f("model", glm::value_ptr(model));
	shader->setMat4f("view", glm::value_ptr(view));
	shader->setMat4f("projection", glm::value_ptr(projection));

    /* Render & Draw */
	GL_CALL(glBindVertexArray(vao)); // Bind Current VAO
    
    /* Draw */
    for (int i = 0; i < 3; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, object_positon[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		shader->setMat4f("model", glm::value_ptr(model));
		GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    }
    //GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    
    /* End Clear */
	GL_CALL(glBindVertexArray(0));  // De-Bind Current VAO
	shader->end();      // De-Bind User Shader
}


int main() {
    bool status = APP->init(800, 600);
    if (!status) return -1;

    // set callback monitor
    APP->setResizeCallback(onResize);
    APP->setKeyboardCallback(onKeyChange);
	APP->setMouseMoveCallback(onMouseMove);
    APP->setMouseScrollCallback(onMouseScroll);
   
    GL_CALL(glViewport(0, 0, 800, 600));
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    //prepareInterleavedBuffer();
    prepareCubeBuffer();
    prepareShader();

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // window loop
    while (APP->update()) {
        render();
    }

    APP->destroy();
    delete camera;
    delete shader;

    return 0;
}