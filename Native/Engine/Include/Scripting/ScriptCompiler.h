#pragma once
#include <Windows.h>
#include "Events.h"

namespace Odyssey
{
	class ScriptCompiler
	{
	public:
		static void ListenForEvents();

	public:
		static bool BuildUserAssembly();
		static bool Process();

	private:
		static bool BuildAssemblies(std::wstring buildCommand);
		static bool WaitForBuildComplete(PROCESS_INFORMATION pi);

	private:
		static void UserFilesModified(OnUserFilesModified* fileSavedEvent);

	private:
		inline static bool buildInProgress = false;
		inline static bool shouldRebuild = false;
	};
}