#pragma once

#include <map>

#include "glew/glew.h"
#include "GLFW/glfw3.h"

#include "Utils.h"

class InputHandler
{
public:
	enum FocusStates {
		FOCUSING_NONE = 0,
		FOCUSING_GAME = 1 << 0,
		FOCUSING_HUD = 1 << 1,
		FOCUSING_EDITOR = 1 << 2,
		FOCUSING_ALL = FOCUSING_EDITOR | FOCUSING_GAME | FOCUSING_HUD,
	};

private :
	static GLFWwindow* m_window;

	static std::map<int, bool> previousMouseState;
	static std::map<int, bool> previousKeyboardState;

	static FocusStates m_focusState;

public:

	static void synchronize();

	static bool getMouseButtonUp(int button, FocusStates focusContext = FocusStates::FOCUSING_ALL);
	static bool getMouseButton(int button, FocusStates focusContext = FocusStates::FOCUSING_ALL);
	static bool getMouseButtonDown(int button, FocusStates focusContext = FocusStates::FOCUSING_ALL);

	static bool getKeyUp(int key, FocusStates focusContext = FocusStates::FOCUSING_ALL);
	static bool getKey(int key, FocusStates focusContext = FocusStates::FOCUSING_ALL);
	static bool getKeyDown(int key, FocusStates focusContext = FocusStates::FOCUSING_ALL);

	static FocusStates getFocusState();
	static void setFocusState(FocusStates state);

	static void attachToWindow(GLFWwindow* window);

private:
	//void _synchronize();

	//bool _getMouseButtonUp(int button);
	//bool _getMouseButton(int button);
	//bool _getMouseButtonDown(int button);

	//bool _getKeyUp(int key);
	//bool _getKey(int key);
	//bool _getKeyDown(int key);

	//FocusStates _getFocusState();
	//void _setFocusState(FocusStates state);

	//void _attachToWindow(GLFWwindow* window);


// singleton implementation :
private:
	InputHandler();
public:
	inline static InputHandler& get()
	{
		static InputHandler instance;

		return instance;
	}


	InputHandler(const InputHandler& other) = delete;
	void operator=(const InputHandler& other) = delete;
};

ENUM_MASK_IMPLEMENTATION(InputHandler::FocusStates)