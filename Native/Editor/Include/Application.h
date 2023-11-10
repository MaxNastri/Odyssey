#pragma once
#include <filesystem>
#include <ScriptingManager.h>
#include <Stopwatch.h>
#include <FileWatcher.h>
#include <VulkanRenderer.h>
#include "InspectorWindow.h"

using namespace Odyssey::Scripting;
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
		std::vector<std::shared_ptr<GUIElement>> guiElements;

		const float MaxFPS = 1.0f / 144.0f;
	};
}