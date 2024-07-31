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
			std::filesystem::path CacheDirectory;
			std::filesystem::path UserScriptsDirectory;
			std::filesystem::path UserScriptsProject;
			std::filesystem::path ApplicationPath;
		};

	public:
		static void Initialize(Settings compilerOptions);

	public:
		static bool BuildUserAssembly();
		static bool Process();
		static const std::filesystem::path& GetUserAssemblyPath() { return m_UserAssemblyPath; }

	private:
		static bool BuildAssemblies(std::wstring buildCommand);
		static bool WaitForBuildComplete(PROCESS_INFORMATION pi);
		static void OnFileAction(const std::filesystem::path& filename, FileActionType fileAction);

	private:
		inline static bool buildInProgress = false;
		inline static bool shouldRebuild = false;
		inline static std::filesystem::path m_UserAssembliesDirectory;
		inline static std::filesystem::path m_UserAssemblyPath;
		inline static std::filesystem::path m_UserAssemblyFilename;
		inline static Settings m_Settings;
		inline static std::unique_ptr<FileTracker> m_FileTracker;
		static constexpr std::string_view USER_ASSEMBLIES_DIRECTORY = "UserAssemblies";
	};
}