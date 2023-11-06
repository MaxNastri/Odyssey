#pragma once
#include <Windows.h>

namespace Odyssey::Scripting
{
	class ScriptCompiler
	{
	public:
		ScriptCompiler();
		bool CompileUserAssembly();
		bool StartBuild(std::wstring buildCommand);
		bool WaitForBuildComplete(PROCESS_INFORMATION pi);
	private:
		bool buildInProgress;
	};
}