#include "ScriptCompiler.h"
#include "Paths.h"
#include <Logger.h>
#include <Windows.h>
#include <EventSystem.h>
#include "EditorEvents.h"
#include <ScriptingManager.h>

namespace Odyssey
{
	bool ScriptCompiler::buildInProgress = false;

	void ScriptCompiler::ListenForEvents()
	{
		EventSystem::Listen<OnUserFilesModified>(ScriptCompiler::UserFilesModified);
	}

	bool ScriptCompiler::BuildUserAssembly()
	{
		if (buildInProgress)
		{
			Logger::LogError("Cannot compile while a build is in progress.");
			return false;
		}

		const char* projectPath = Paths::Relative::ExampleManagedProject;

		std::wstring buildCommand = L" build \"" +
			std::filesystem::absolute(projectPath).wstring() +
			L"\" -c Debug --no-self-contained " +
			L"-o \"./tmp_build/\" -r \"win-x64\"";

		bool success = BuildAssemblies(buildCommand);
		EventSystem::Dispatch<OnBuildFinished>(success);

		if (success)
		{
			Scripting::ScriptingManager::ReloadUserAssemblies();
		}

		return success;
	}

	bool ScriptCompiler::BuildAssemblies(std::wstring buildCommand)
	{
		buildInProgress = true;

		EventSystem::Dispatch<OnBuildStart>();

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
			Odyssey::Logger::LogError(oss.view());

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
				oss << "Failed to query process. Error code: " << std::hex << err;
				Odyssey::Logger::LogError(oss.view());
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
			Odyssey::Logger::LogError("Failed to build managed scripts!");
			return false;
		}
	}

	void ScriptCompiler::UserFilesModified(OnUserFilesModified* fileSavedEvent)
	{
		if (!buildInProgress)
		{
			for (const auto& changedFile : fileSavedEvent->changedFileSet)
			{
				if (changedFile.second != FileNotifcations::RenamedNew &&
					changedFile.second != FileNotifcations::RenamedOld)
				{
					BuildUserAssembly();
					break;
				}
			}
		}
	}
}