#pragma once
#include "glm.h"
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
		static bool GetMouseButtonDown(MouseButton button);
		static double GetMouseAxisHorizontal() { return mouseAxisH; }
		static double GetMouseAxisVerticle() { return mouseAxisV; }
		static glm::vec2 GetMousePosition() { return glm::vec2(mouseX, mouseY); }
		static glm::vec2 GetScreenSpaceMousePosition();

	public:
		static void Update();
		static void RegisterKeyPress(int key, int scanCode);
		static void RegisterKeyDown(int key, int scanCode);
		static void RegisterKeyUp(int key, int scanCode);
		static void RegisterMouseMove(double x, double y);
		static void RegisterMousePosition(double x, double y, bool entered);
		static void RegisterMouseClick(MouseButton button, bool pressed);
	private:
		enum KeyState
		{
			Press = 0,
			Down = 1,
			Up = 2,
			Size = 3,
		};

		inline static std::array<std::bitset<1024>, KeyState::Size> keyInput;
		inline static std::array<bool, MouseButton::Size> mouseInput;
		inline static double mouseX = 0.0, mouseY = 0.0;
		inline static double prevMouseX = 0.0, prevMouseY = 0.0;
		inline static double mouseAxisH = 0.0, mouseAxisV = 0.0;
	};
}