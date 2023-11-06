#pragma once
#include <filesystem>
#include <ScriptingManager.h>
#include <Stopwatch.h>
#include <Window.h>
#include <FileWatcher.h>

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
		Window window;
		Stopwatch stopwatch;

		const float MaxFPS = 1.0f / 144.0f;
	};
}