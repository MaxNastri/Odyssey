#pragma once
#include <filesystem>
#include <ScriptingManager.h>
#include <Stopwatch.h>
#include <Window.h>

using namespace Odyssey::Scripting;
using namespace Odyssey::Framework;

namespace Odyssey::Editor
{
	class Application
	{
	public:
		Application(std::filesystem::path path);
		void Run();
		void Exit();

	private:
		bool running;
		bool allowRecompile = true;
		ScriptingManager scriptingManager;
		Window window;
		Stopwatch stopwatch;

		const float MaxFPS = 1.0f / 144.0f;
	};
}