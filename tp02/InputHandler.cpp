#include "InputHandler.h"

GLFWwindow* InputHandler::m_window = nullptr;
std::map<int, bool> InputHandler::previousMouseState;
std::map<int, bool> InputHandler::previousKeyboardState;
InputHandler::FocusStates InputHandler::m_focusState = InputHandler::FOCUSING_ALL;


InputHandler::InputHandler()
{
	setFocusState(FocusStates::FOCUSING_ALL);
	attachToWindow(nullptr);
}

void InputHandler::synchronize()
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

bool InputHandler::getMouseButtonUp(int button, FocusStates focusContext)
{
	return (!glfwGetMouseButton(m_window, button) && previousMouseState[button]) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

bool InputHandler::getMouseButton(int button, FocusStates focusContext)
{
	return glfwGetMouseButton(m_window, button) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

bool InputHandler::getMouseButtonDown(int button, FocusStates focusContext)
{
	return (glfwGetMouseButton(m_window, button) && !previousMouseState[button]) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

bool InputHandler::getKeyUp(int key, FocusStates focusContext)
{
	return (!glfwGetKey(m_window, key) && previousKeyboardState[key]) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

bool InputHandler::getKey(int key, FocusStates focusContext)
{
	return glfwGetKey(m_window, key) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

bool InputHandler::getKeyDown(int key, FocusStates focusContext)
{
	return (glfwGetKey(m_window, key) && !previousKeyboardState[key]) && ((focusContext & m_focusState) != FocusStates::FOCUSING_NONE);
}

InputHandler::FocusStates InputHandler::getFocusState()
{
	return m_focusState;
}

void InputHandler::setFocusState(FocusStates state)
{
	m_focusState = state;
}

void InputHandler::attachToWindow(GLFWwindow * window)
{
	m_window = window;
}

//void InputHandler::synchronize()
//{
//	get()._synchronize();
//}
//bool InputHandler::getMouseButtonUp(int button, FocusStates focusContext)
//{
//	return get()._getMouseButtonUp(button);
//}
//
//bool InputHandler::getMouseButton(int button, FocusStates focusContext)
//{
//	return get()._getMouseButton(button);
//}
//
//bool InputHandler::getMouseButtonDown(int button, FocusStates focusContext)
//{
//	return get()._getMouseButtonDown(button);
//}
//
//bool InputHandler::getKeyUp(int key, FocusStates focusContext)
//{
//	return get()._getKeyUp(key);
//}
//
//bool InputHandler::getKey(int key, FocusStates focusContext)
//{
//	return get()._getKey(key);
//}
//
//bool InputHandler::getKeyDown(int key, FocusStates focusContext)
//{
//	return get()._getKeyDown(key);
//}
//
//InputHandler::FocusStates InputHandler::getFocusState()
//{
//	return get()._getFocusState();
//}
//
//void InputHandler::setFocusState(InputHandler::FocusStates state)
//{
//	return get()._setFocusState(state);
//}
//
//void InputHandler::attachToWindow(GLFWwindow * window)
//{
//	get()._attachToWindow(window);
//}

//void InputHandler::_synchronize()
//{
//	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
//	{
//		previousMouseState[i] = glfwGetMouseButton(m_window, i);
//	}
//	for (int i = 0; i < GLFW_KEY_LAST; i++)
//	{
//		previousKeyboardState[i] = glfwGetKey(m_window, i);
//	}
//}
//
//bool InputHandler::_getMouseButtonUp(int button)
//{
//	return (!glfwGetMouseButton(m_window, button) && previousMouseState[button]);
//}
//
//bool InputHandler::_getMouseButton(int button)
//{
//	return glfwGetMouseButton(m_window, button);
//}
//
//bool InputHandler::_getMouseButtonDown(int button)
//{
//	return (glfwGetMouseButton(m_window, button) && !previousMouseState[button]);
//}
//
//bool InputHandler::_getKeyUp(int key)
//{
//	return (!glfwGetKey(m_window, key) && previousKeyboardState[key]);
//}
//
//bool InputHandler::_getKey(int key)
//{
//	return glfwGetKey(m_window, key);
//}
//
//bool InputHandler::_getKeyDown(int key)
//{
//	return (glfwGetKey(m_window, key) && !previousKeyboardState[key]);
//}
//
//InputHandler::FocusStates InputHandler::_getFocusState()
//{
//	return m_focusState;
//}
//
//void InputHandler::_setFocusState(InputHandler::FocusStates state)
//{
//	m_focusState = state;
//}
//
//void InputHandler::_attachToWindow(GLFWwindow * window)
//{
//	m_window = window;
//}
