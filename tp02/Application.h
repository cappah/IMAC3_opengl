#pragma once


class Application {

private:
	int m_windowWidth;
	int m_windowHeight;

	bool m_windowResize;

public:
	void setWindowHeight(int height);

	void setWindowWidth(int width);

	int getWindowHeight();

	int getWindowWidth();

	bool getWindowResize();

	void setWindowResize(bool state);


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
