#pragma once

#include <map>

#include "glew/glew.h"
#include "GLFW/glfw3.h"

class InputHandler
{
private :
	GLFWwindow* m_window;

	std::map<int, bool> previousMouseState;
	std::map<int, bool> previousKeyboardState;

public:

	static void synchronize();

	static bool getMouseButtonUp(int button);
	static bool getMouseButton(int button);
	static bool getMouseButtonDown(int button);

	static bool getKeyUp(int key);
	static bool getKey(int key);
	static bool getKeyDown(int key);


	static void attachToWindow(GLFWwindow* window);

private:
	void _synchronize();

	bool _getMouseButtonUp(int button);
	bool _getMouseButton(int button);
	bool _getMouseButtonDown(int button);

	bool _getKeyUp(int key);
	bool _getKey(int key);
	bool _getKeyDown(int key);

	void _attachToWindow(GLFWwindow* window);


// singleton implementation :
private:
	InputHandler() {

	}

public:
	inline static InputHandler& get()
	{
		static InputHandler instance;

		return instance;
	}


	InputHandler(const InputHandler& other) = delete;
	void operator=(const InputHandler& other) = delete;
};

