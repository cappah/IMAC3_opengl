#include "Application.h"

void Application::setWindowHeight(int height)
{
	m_windowHeight = height;
}

void Application::setWindowWidth(int width)
{
	m_windowWidth = width;
}

int Application::getWindowHeight()
{
	return m_windowHeight;
}

int Application::getWindowWidth()
{
	return m_windowWidth;
}

bool Application::getWindowResize()
{
	return m_windowResize;
}

void Application::setWindowResize(bool state)
{
	m_windowResize = state;
}

double Application::getTime() const
{
	return glfwGetTime();
}

void Application::setDeltaTime(double deltaTime)
{
	m_deltaTime = deltaTime;
}

double Application::getDeltaTime() const
{
	return m_deltaTime;
}

