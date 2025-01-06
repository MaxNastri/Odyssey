#include "Window.h"
#include "Input.h"
#include <Log.h>

// GLFW3 Includes
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>
#endif

namespace Odyssey
{
	Window::Window()
	{
		title = "Odyssey Engine";
		m_Width = 1920;
		m_Height = 1080;

		// Initialize the glfw library
		glfwInit();

		// Tip: By default glfw targets an OpenGL context. We include this line to clear the API setting of OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		// Start the window maximized
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		// Create the glfw window and store the glfw + windows handles
		glfwHandle = glfwCreateWindow(m_Width, m_Height, title.c_str(), nullptr, nullptr);

		glfwSetKeyCallback(glfwHandle, Window::KeyCallback);
		glfwSetCursorPosCallback(glfwHandle, Window::MouseMoveCallback);
		glfwSetCursorEnterCallback(glfwHandle, Window::MouseEnteredCallback);
		glfwSetMouseButtonCallback(glfwHandle, Window::MouseButtonClicked);
		glfwSetWindowPosCallback(glfwHandle, Window::WindowMoved);

		int xpos, ypos;
		glfwGetWindowPos(glfwHandle, &xpos, &ypos);
		s_WindowPos = glm::vec2(xpos, ypos);

		// Store the current DPI
		float xScale, yScale;
		glfwGetWindowContentScale(glfwHandle, &xScale, &yScale);

		m_ContentScale = std::max(xScale, yScale);
	}

	bool Window::Update()
	{
		Input::Update();
		glfwPollEvents();
		return !ShouldClose();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(glfwHandle);
	}

	void Window::CaptureCursor()
	{
		glfwSetInputMode(glfwHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void Window::ReleaseCursor()
	{
		glfwSetInputMode(glfwHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	Window::~Window()
	{
		if (glfwHandle)
		{
			glfwDestroyWindow(glfwHandle);
		}
	}

	void Window::GetFrameBufferSize(int& width, int& height)
	{
		glfwGetFramebufferSize(glfwHandle, &width, &height);

		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(glfwHandle, &width, &height);
			glfwWaitEvents();
		}
	}

	void Window::SetSize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
	}

	void Window::ErrorCallback(int error, const char* description)
	{
		Log::Error(std::format("GLFW Error %d: %s\n", error, description));
	}

	void Window::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			// Key press
			Input::RegisterKeyPress(key, scanCode);
		}
		else if (action == GLFW_REPEAT)
		{
			// DOWN
			Input::RegisterKeyDown(key, scanCode);
		}
		else if (action == GLFW_RELEASE)
		{
			// Up
			Input::RegisterKeyUp(key, scanCode);
		}
	}

	void Window::MouseMoveCallback(GLFWwindow* window, double x, double y)
	{
		Input::RegisterMouseMove((float)x, (float)y);
	}

	void Window::MouseEnteredCallback(GLFWwindow* window, int entered)
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		Input::RegisterMousePosition((float)xPos, (float)yPos, entered);
	}

	void Window::MouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
	{
		MouseButton mouseButton = button == GLFW_MOUSE_BUTTON_LEFT ? MouseButton::Left : MouseButton::Right;
		bool pressed = action == GLFW_PRESS;

		Input::RegisterMouseClick(mouseButton, pressed);
	}

	void Window::WindowMoved(GLFWwindow* window, int xpos, int ypos)
	{
		s_WindowPos = glm::vec2(xpos, ypos);
	}
}