#pragma once
#include "FileTracker.h"

namespace Odyssey
{
	class ScriptCompiler
	{
	public:
		struct Settings
		{
		public:
			Path CacheDirectory;
			Path UserScriptsDirectory;
			Path UserScriptsProject;
			Path ApplicationPath;
		};

	public:
		ScriptCompiler(const Settings& settings);

	public:
		bool BuildUserAssembly();
		bool Process();
		const Path& GetUserAssemblyPath() { return m_UserAssemblyPath; }

	private:
		bool BuildAssemblies(std::wstring buildCommand);
		bool WaitForBuildComplete(PROCESS_INFORMATION pi);
		void OnFileAction(const Path& filename, FileActionType fileAction);

	private:
		bool buildInProgress = false;
		bool shouldRebuild = false;
		Path m_UserAssembliesDirectory;
		Path m_UserAssemblyPath;
		Path m_UserAssemblyFilename;
		Settings m_Settings;
		std::unique_ptr<FileTracker> m_FileTracker;
		static constexpr std::string_view USER_ASSEMBLIES_DIRECTORY = "UserAssemblies";
		static constexpr std::string_view SCRIPTS_RESOURCES_DIRECTORY = "Resources/Scripts";
	};
}