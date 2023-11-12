#pragma once
#include <Windows.h>
#include "EditorEvents.h"

namespace Odyssey::Editor
{
	class ScriptCompiler
	{
	public:
		static void ListenForEvents();

	public:
		static bool BuildUserAssembly();

	private:
		static bool BuildAssemblies(std::wstring buildCommand);
		static bool WaitForBuildComplete(PROCESS_INFORMATION pi);

	private:
		static void UserFilesModified(OnUserFilesModified* fileSavedEvent);

	private:
		static bool buildInProgress;
	};
}