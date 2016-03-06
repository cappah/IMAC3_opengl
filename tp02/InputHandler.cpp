#include "InputHandler.h"


void InputHandler::synchronize()
{
	get()._synchronize();
}

bool InputHandler::getMouseButtonUp(int button)
{
	return get()._getMouseButtonUp(button);
}

bool InputHandler::getMouseButton(int button)
{
	return get()._getMouseButton(button);
}

bool InputHandler::getMouseButtonDown(int button)
{
	return get()._getMouseButtonDown(button);
}

bool InputHandler::getKeyUp(int key)
{
	return get()._getKeyUp(key);
}

bool InputHandler::getKey(int key)
{
	return get()._getKey(key);
}

bool InputHandler::getKeyDown(int key)
{
	return get()._getKeyDown(key);
}

void InputHandler::attachToWindow(GLFWwindow * window)
{
	get()._attachToWindow(window);
}

void InputHandler::_synchronize()
{
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
	{
		previousMouseState[i] = glfwGetMouseButton(m_window, i);
	}
	for (int i = 0; i < GLFW_KEY_LAST; i++)
	{
		previousKeyboardState[i] = glfwGetKey(m_window, i);
	}
}

bool InputHandler::_getMouseButtonUp(int button)
{
	return (!glfwGetMouseButton(m_window, button) && previousMouseState[button]);
}

bool InputHandler::_getMouseButton(int button)
{
	return glfwGetMouseButton(m_window, button);
}

bool InputHandler::_getMouseButtonDown(int button)
{
	return (glfwGetMouseButton(m_window, button) && !previousMouseState[button]);
}

bool InputHandler::_getKeyUp(int key)
{
	return (!glfwGetKey(m_window, key) && previousKeyboardState[key]);
}

bool InputHandler::_getKey(int key)
{
	return glfwGetKey(m_window, key);
}

bool InputHandler::_getKeyDown(int key)
{
	return (glfwGetKey(m_window, key) && !previousKeyboardState[key]);
}

void InputHandler::_attachToWindow(GLFWwindow * window)
{
	m_window = window;
}
