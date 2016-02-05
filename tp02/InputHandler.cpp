#include "InputHandler.h"



InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
}

void InputHandler::synchronize(GLFWwindow* window)
{
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
	{
		previousMouseState[i] = glfwGetMouseButton(window, i);
	}
	for (int i = 0; i < GLFW_KEY_LAST; i++)
	{
		previousKeyboardState[i] = glfwGetKey(window, i);
	}
}

bool InputHandler::getMouseButtonUp(GLFWwindow* window, int button)
{
	return (!glfwGetMouseButton(window, button) && previousMouseState[button]);
}

bool InputHandler::getMouseButton(GLFWwindow* window, int button)
{
	return glfwGetMouseButton(window, button);
}

bool InputHandler::getMouseButtonDown(GLFWwindow* window, int button)
{
	return (glfwGetMouseButton(window, button) && !previousMouseState[button]);
}

bool InputHandler::getKeyUp(GLFWwindow* window, int key)
{
	return (!glfwGetKey(window, key) && previousKeyboardState[key]);
}

bool InputHandler::getKey(GLFWwindow* window, int key)
{
	return glfwGetKey(window, key);
}

bool InputHandler::getKeyDown(GLFWwindow* window, int key)
{
	return (glfwGetKey(window, key) && !previousKeyboardState[key]);
}
