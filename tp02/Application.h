#pragma once


class Application {

private:
	int windowWidth;
	int windowHeight;

public:
	void setWindowHeight(int height);

	void setWindowWidth(int width);

	int getWindowHeight();

	int getWindowWidth();


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
