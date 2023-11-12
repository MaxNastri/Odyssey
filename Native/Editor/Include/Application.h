#pragma once
#include <VulkanRenderer.h>
#include <Stopwatch.h>

using namespace Odyssey::Framework;
using namespace Odyssey::Graphics;

namespace Odyssey::Graphics
{
	class GUIElement;
}

namespace Odyssey::Editor
{
	class Application
	{
	public:
		Application();
		void Run();
		void Exit();

	private:
		bool running;
		bool allowRecompile = true;
		Stopwatch stopwatch;
		VulkanRenderer r;

		const float MaxFPS = 1.0f / 144.0f;
	};
}