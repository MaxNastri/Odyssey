#pragma once
#include <string>

// FWD Declarations
struct GLFWwindow;

namespace Odyssey
{
	class Window
	{
	public:
		Window();
		bool Update();
		bool ShouldClose();
		~Window();

	public:
		GLFWwindow* GetWindowHandle() { return glfwHandle; }

	private:
		static void ErrorCallback(int error, const char* description);
		static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double x, double y);
		static void MouseEnteredCallback(GLFWwindow* window, int entered);
		static void WindowResize(GLFWwindow* window, int width, int height);

	private:
		GLFWwindow* glfwHandle;

		// Properties
		unsigned int width, height;
		std::string title;
	};
}