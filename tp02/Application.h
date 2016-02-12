#pragma once

#include "GLFW/glfw3.h"

class Application {

private:
	int m_windowWidth;
	int m_windowHeight;

	bool m_windowResize;

	double m_deltaTime;

public:
	void setWindowHeight(int height);

	void setWindowWidth(int width);

	int getWindowHeight();

	int getWindowWidth();

	bool getWindowResize();

	void setWindowResize(bool state);

	//elapsed time since the beginning of the program, in seconds
	double getTime() const;

	void setDeltaTime(double deltaTime);

	double getDeltaTime() const;


	// singleton implementation :
private:
	Application() {

	}

public:
	inline static Application& get()
	{
		static Application instance;

		return instance;
	}


	Application(const Application& other) = delete;
	void operator=(const Application& other) = delete;

};
