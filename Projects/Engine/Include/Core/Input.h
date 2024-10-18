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
		static float GetMouseAxisHorizontal() { return mouseAxisH; }
		static float GetMouseAxisVertical() { return mouseAxisV; }
		static glm::vec2 GetMousePosition() { return glm::vec2(mouseX, mouseY); }
		static glm::vec2 GetScreenSpaceMousePosition();
		static bool IsMouseDragging();

	public:
		static void Update();
		static void RegisterKeyPress(int key, int scanCode);
		static void RegisterKeyDown(int key, int scanCode);
		static void RegisterKeyUp(int key, int scanCode);
		static void RegisterMouseMove(float x, float y);
		static void RegisterMousePosition(float x, float y, bool entered);
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
		inline static float mouseX = 0.0, mouseY = 0.0;
		inline static float prevMouseX = 0.0, prevMouseY = 0.0;
		inline static float mouseAxisH = 0.0, mouseAxisV = 0.0;
	};
}