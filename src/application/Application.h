#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define APP (Application::getInstance())

//class GLFWwindow;

using ResizeCB = void(*)(int, int);
using KeyboardCB = void(*)(int, int, int, int);

/* Application  
 *	- for windows encapsulation
 *  - Singleton
 */
class Application {
public:
	~Application();
	static Application* getInstance();

	uint32_t getWidth()  const { return _width; }
	uint32_t getHeight() const { return _height; }
	
	/* Main Operation */
	bool init(const int&, const int&);
	bool update();
	void destroy();

	/* Set Event Callback */
	void setResizeCallback(ResizeCB cb) { _resize_cb = cb; }
	void setKeyboardCallback(KeyboardCB cb) { _keyboard_cb = cb; }

private:
	uint32_t _width  = 0;
	uint32_t _height = 0;
	GLFWwindow* _window = nullptr;

	/* Event Callback */
	ResizeCB _resize_cb = nullptr;
	static void fbSizeCallback(GLFWwindow* window, int width, int height);
	KeyboardCB _keyboard_cb = nullptr;
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


	static Application* mInstance;
	Application();
};

#endif // !APPLICATION_H