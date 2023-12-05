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
		void GetFrameBufferSize(int& x, int& y);
		void SetSize(uint32_t width, uint32_t height);

	private:
		static void ErrorCallback(int error, const char* description);
		static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double x, double y);
		static void MouseEnteredCallback(GLFWwindow* window, int entered);
		static void MouseButtonClicked(GLFWwindow* window, int button, int action, int mods);
		static void WindowResize(GLFWwindow* window, int width, int height);

	private:
		GLFWwindow* glfwHandle;
		// Properties
		uint32_t m_Width, m_Height;
		std::string title;
	};
}