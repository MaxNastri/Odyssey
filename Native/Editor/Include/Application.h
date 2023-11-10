#pragma once
#include <filesystem>
#include <ScriptingManager.h>
#include <Stopwatch.h>
#include <FileWatcher.h>
#include <VulkanRenderer.h>

using namespace Odyssey::Scripting;
using namespace Odyssey::Framework;

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
		Graphics::VulkanRenderer r;
		const float MaxFPS = 1.0f / 144.0f;
	};
}