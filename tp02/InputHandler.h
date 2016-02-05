#pragma once

#include <map>

#include "glew/glew.h"
#include "GLFW/glfw3.h"

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	std::map<int, bool> previousMouseState;
	std::map<int, bool> previousKeyboardState;

	void synchronize(GLFWwindow* window);

	bool getMouseButtonUp(GLFWwindow* window, int button);
	bool getMouseButton(GLFWwindow* window, int button);
	bool getMouseButtonDown(GLFWwindow* window, int button);

	bool getKeyUp(GLFWwindow* window, int key);
	bool getKey(GLFWwindow* window, int key);
	bool getKeyDown(GLFWwindow* window, int key);
};

