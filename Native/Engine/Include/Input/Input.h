#pragma once
#include <bitset>
#include "KeyCode.h"

// FWD Declarations
struct GLFWwindow;

namespace Odyssey
{
	class Window;

	class Input
	{
	public:
		static void Initialize();

	public:
		static bool GetKeyPress(KeyCode keyCode);
		static bool GetKeyDown(KeyCode keyCode);
		static bool GetKeyUp(KeyCode keyCode);

	public:
		static void RegisterKeyPress(int key, int scanCode);
		static void RegisterKeyDown(int key, int scanCode);
		static void RegisterKeyUp(int key, int scanCode);
		static void RegisterMouseMove(double x, double y);
		static void RegisterMousePosition(double x, double y, bool entered);

	private:
		enum KeyState
		{
			Press = 0,
			Down = 1,
			Up = 2,
			Size = 3,
		};

		static std::array<std::bitset<1024>, KeyState::Size> keyInput;
		static double mouseX, mouseY;
	};
}