#include "ScriptCompiler.h"
#include "Log.h"
#include "EventSystem.h"
#include "Events.h"
#include "ScriptingManager.h"

namespace Odyssey
{
	ScriptCompiler::ScriptCompiler(const Settings& settings)
		: m_Settings(settings)
	{
		// Construct the necessary assembly paths
		m_UserAssembliesDirectory = m_Settings.CacheDirectory / USER_ASSEMBLIES_DIRECTORY;
		m_UserAssemblyFilename = m_Settings.UserScriptsProject.filename().replace_extension(".dll");
		m_UserAssemblyPath = m_UserAssembliesDirectory / m_UserAssemblyFilename;

		if (!std::filesystem::exists(m_UserAssembliesDirectory))
			std::filesystem::create_directories(m_UserAssembliesDirectory);

		// Keep the user assemblies directory up to date with our latest coral and framework dlls
		std::filesystem::copy(SCRIPTS_RESOURCES_DIRECTORY, m_UserAssembliesDirectory, std::filesystem::copy_options::overwrite_existing);

		TrackingOptions options;
		options.TrackingPath = m_Settings.UserScriptsDirectory;
		options.Extensions = { ".cs" };
		options.Recursive = true;
		options.Callback = [this](const Path& oldPath, const Path& newPath, FileActionType fileAction)
			{ OnFileAction(oldPath, newPath, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);
	}

	bool ScriptCompiler::BuildUserAssembly()
	{
		if (buildInProgress)
		{
			Log::Error("Cannot compile while a build is in progress.");
			return false;
		}

		std::wstring buildCommand = L" build \"" +
			m_Settings.UserScriptsProject.wstring() +
			L"\" -c Debug";

		bool success = BuildAssemblies(buildCommand);
		EventSystem::Dispatch<BuildCompleteEvent>(success);

		if (success)
		{
			ScriptingManager::ReloadAssemblies();
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
			Odyssey::Log::Error(oss.view());

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
				Odyssey::Log::Error(oss.view());
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
			return true;
		}
		// Failed build
		else
		{
			Odyssey::Log::Error("Failed to build managed scripts!");
			return false;
		}
	}

	void ScriptCompiler::OnFileAction(const Path& oldFilename, const Path& newFilename, FileActionType fileAction)
	{
		shouldRebuild = !buildInProgress && fileAction != FileActionType::None;
	}
}