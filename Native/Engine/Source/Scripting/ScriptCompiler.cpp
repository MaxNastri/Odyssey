#include "ScriptCompiler.h"
#include <Windows.h>
#include "Logger.h"
#include "EventSystem.h"
#include "Events.h"
#include "ScriptingManager.h"
#include <locale>
#include <codecvt>

namespace Odyssey
{
	void ScriptCompiler::Initialize(Settings compilerSettings)
	{
		m_Settings = compilerSettings;

		// Construct the necessary assembly paths
		m_UserAssembliesDirectory = m_Settings.CacheDirectory / USER_ASSEMBLIES_DIRECTORY;
		m_UserAssemblyFilename = m_Settings.UserScriptsProject.filename().replace_extension(".dll");
		m_UserAssemblyPath = m_UserAssembliesDirectory / m_UserAssemblyFilename;
		
		if (!std::filesystem::exists(m_UserAssembliesDirectory))
			std::filesystem::create_directories(m_UserAssembliesDirectory);

		EventSystem::Listen<OnUserFilesModified>(ScriptCompiler::UserFilesModified);
	}

	bool ScriptCompiler::BuildUserAssembly()
	{
		if (buildInProgress)
		{
			Logger::LogError("Cannot compile while a build is in progress.");
			return false;
		}


		std::wstring buildCommand = L" build \"" +
			m_Settings.UserScriptsProject.wstring() +
			L"\" -c Debug --no-self-contained " +
			L"-o \"" + m_UserAssembliesDirectory.wstring() + L"\" -r \"win-x64\"";

		bool success = BuildAssemblies(buildCommand);
		EventSystem::Dispatch<OnBuildFinished>(success);

		if (success)
		{
			ScriptingManager::ReloadUserAssemblies();
		}

		return success;
	}

	bool ScriptCompiler::Process()
	{
		if (shouldRebuild)
		{
			shouldRebuild = false;
			return BuildUserAssembly();
		}

		return true;
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
			std::filesystem::path outputPath = m_Settings.ApplicationPath / m_UserAssemblyFilename;
			std::filesystem::copy(m_UserAssemblyPath, outputPath,
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
				std::filesystem::path path(changedFile.first);

				if (path.extension() == ".cs" &&
					changedFile.second != FileNotifcations::RenamedNew &&
					changedFile.second != FileNotifcations::RenamedOld)
				{
					shouldRebuild = true;
					break;
				}
			}
		}
	}
}