#pragma once
#include "glm.h"

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

		static glm::vec2 GetWindowPos() { return s_WindowPos; }

	private:
		static void ErrorCallback(int error, const char* description);
		static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
		static void MouseMoveCallback(GLFWwindow* window, double x, double y);
		static void MouseEnteredCallback(GLFWwindow* window, int entered);
		static void MouseButtonClicked(GLFWwindow* window, int button, int action, int mods);
		static void WindowMoved(GLFWwindow* window, int xpos, int ypos);

	private:
		GLFWwindow* glfwHandle;
		// Properties
		uint32_t m_Width, m_Height;
		inline static glm::vec2 s_WindowPos;
		std::string title;
	};
}