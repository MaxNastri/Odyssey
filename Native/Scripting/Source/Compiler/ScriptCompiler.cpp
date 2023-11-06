#include "ScriptCompiler.h"
#include "Paths.h"
#include <Log.h>
#include <Windows.h>

namespace Odyssey::Scripting
{
	ScriptCompiler::ScriptCompiler()
	{
		buildInProgress = false;
	}

	bool ScriptCompiler::CompileUserAssembly()
	{
		if (buildInProgress)
		{
			Odyssey::Framework::Log::Error("Cannot compile while a build is in progress.");
			return false;
		}

		const char* projectPath = Paths::Relative::ExampleManagedProject;

		std::wstring buildCommand = L" build \"" +
			std::filesystem::absolute(projectPath).wstring() +
			L"\" -c Debug --no-self-contained " +
			L"-o \"./tmp_build/\" -r \"win-x64\"";

		return StartBuild(buildCommand);
	}

	bool ScriptCompiler::StartBuild(std::wstring buildCommand)
	{
		buildInProgress = true;

		STARTUPINFOW startInfo;
		PROCESS_INFORMATION pi;
		ZeroMemory(&startInfo, sizeof(startInfo));
		ZeroMemory(&pi, sizeof(pi));
		startInfo.cb = sizeof(startInfo);

		const auto SUCCESS = CreateProcess
		(
			L"C:\\Program Files\\dotnet\\dotnet.exe", buildCommand.data(),
			nullptr, nullptr, true, NULL, nullptr, nullptr,
			&startInfo, &pi
		);

		if (!SUCCESS)
		{
			auto err = GetLastError();

			std::ostringstream oss;
			oss << "Failed to query process. Error code: " << std::hex << err;
			Odyssey::Framework::Log::Error(oss.view());

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			buildInProgress = false;
			return false;
		}

		return WaitForBuildComplete(pi);
	}

	bool ScriptCompiler::WaitForBuildComplete(PROCESS_INFORMATION pi)
	{
		// Wait for process to end
		DWORD exitCode{};
		while (true)
		{
			const auto EXEC_SUCCESS =
				GetExitCodeProcess(pi.hProcess, &exitCode);
			if (!EXEC_SUCCESS)
			{
				auto err = GetLastError();
				std::ostringstream oss;
				oss << "Failed to query process. Error code: "<< std::hex << err;
				Odyssey::Framework::Log::Error(oss.view());
				buildInProgress = false;
				return false;
			}
			if (exitCode != STILL_ACTIVE)
				break;
		}

		// Clean up
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		buildInProgress = false;

		// Successful build
		if (exitCode == 0)
		{
			// Copy out files
			std::filesystem::path targetPath = Paths::Absolute::GetApplicationPath() / "Odyssey.Managed.Example.dll";
			std::filesystem::copy("./tmp_build/Odyssey.Managed.Example.dll", targetPath,
				std::filesystem::copy_options::overwrite_existing);
			return true;
		}
		// Failed build
		else
		{
			Odyssey::Framework::Log::Error("Failed to build managed scripts!");
			return false;
		}
	}
}