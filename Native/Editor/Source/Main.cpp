#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <functional>
#include <ranges>

#include <ScriptingManager.h>
#include "Application.h"

int main(int argc, char** argv)
{
	Odyssey::Editor::Application editor;
	editor.Run();
}