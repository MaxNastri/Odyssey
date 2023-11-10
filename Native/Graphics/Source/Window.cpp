#include "Window.h"
#include "Input.h"
#include <Log.h>

// GLFW3 Includes
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>
#endif

namespace Odyssey::Graphics
{
	Window::Window()
	{
		title = "Odyssey Engine";
		width = 1920;
		height = 1080;

		// Initialize the glfw library
		glfwInit();

		// Tip: By default glfw targets an OpenGL context. We include this line to clear the API setting of OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Create the glfw window and store the glfw + windows handles
		glfwHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		glfwSetKeyCallback(glfwHandle, Window::KeyCallback);
		glfwSetCursorPosCallback(glfwHandle, Window::MouseMoveCallback);
		glfwSetCursorEnterCallback(glfwHandle, Window::MouseEnteredCallback);
		glfwSetFramebufferSizeCallback(glfwHandle, Window::WindowResize);
	}

	bool Window::Update()
	{
		glfwPollEvents();
		return !ShouldClose();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(glfwHandle);
	}

	Window::~Window()
	{
		if (glfwHandle)
		{
			glfwDestroyWindow(glfwHandle);
		}
	}

	void Window::ErrorCallback(int error, const char* description)
	{
		Framework::Log::Error(std::format("GLFW Error %d: %s\n", error, description));
	}

	void Window::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			// Key press
			Framework::Input::RegisterKeyPress(key, scanCode);
		}
		else if (action == GLFW_REPEAT)
		{
			// DOWN
			Framework::Input::RegisterKeyDown(key, scanCode);
		}
		else if (action == GLFW_RELEASE)
		{
			// Up
			Framework::Input::RegisterKeyUp(key, scanCode);
		}
	}

	void Window::MouseMoveCallback(GLFWwindow* window, double x, double y)
	{
		Framework::Input::RegisterMouseMove(x, y);
	}

	void Window::MouseEnteredCallback(GLFWwindow* window, int entered)
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		Framework::Input::RegisterMousePosition(xPos, yPos, entered);
	}

	void Window::WindowResize(GLFWwindow* window, int width, int height)
	{

	}
}