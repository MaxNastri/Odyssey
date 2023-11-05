#pragma once
#include "Assembly.hpp"

namespace Odyssey::Scripting
{
	class ScriptCompiler
	{
	public:
		static void CompileAssembly();

	private:
		static void StartBuild(std::wstring buildCommand);
		static void WaitForBuildComplete(PROCESS_INFORMATION pi);

	private:
		static bool buildInProgress;
	};
}