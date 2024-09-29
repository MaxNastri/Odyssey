#pragma once
#include "ProjectSettings.h"

namespace Odyssey
{
	class Project
	{
	public:
		Project(const ProjectSettings& projectSettings);

	public:
		const Path& GetProjectDirectory() { return m_ProjectSettings.GetProjectDirectory(); }
		const Path& GetAssetsDirectory() { return m_ProjectSettings.GetAssetsDirectory(); }
		const Path& GetCacheDirectory() { return m_ProjectSettings.GetCacheDirectory(); }
		const Path& GetLogsDirectory() { return m_ProjectSettings.GetLogsDirectory(); }
		const Path& GetUserScriptsDirectory() { return m_ProjectSettings.GetScriptsDirectory(); }
		const Path& GetUserScriptsProject() { return m_ProjectSettings.GetScriptsProjectPath(); }
		const Path& GetTempDirectory() { return m_ProjectSettings.GetTempDirectory(); }
		const Path& GetAssetRegistry() { return m_ProjectSettings.GetAssetRegistryPath(); }

	public:
		static void CreateNewProject(const std::string& projectName, const Path& projectDirectory);
		static void LoadProject(const Path& projectDirectory);
		static void SetActive(const ProjectSettings& projectSettings);
		static std::shared_ptr<Project> GetActive();

	public:
		static const Path& GetActiveProjectDirectory() { return s_ActiveProject->GetProjectDirectory(); }
		static const Path& GetActiveAssetsDirectory() { return s_ActiveProject->GetAssetsDirectory(); }
		static const Path& GetActiveCacheDirectory() { return s_ActiveProject->GetCacheDirectory(); }
		static const Path& GetActiveLogsDirectory() { return s_ActiveProject->GetLogsDirectory(); }
		static const Path& GetActiveUserScriptsDirectory() { return s_ActiveProject->GetUserScriptsDirectory(); }
		static const Path& GetActiveUserScriptsProject() { return s_ActiveProject->GetUserScriptsProject(); }
		static const Path& GetActiveTempDirectory() { return s_ActiveProject->GetTempDirectory(); }
		static const Path& GetActiveAssetRegistry() { return s_ActiveProject->GetAssetRegistry(); }

	private:
		static void ReplaceProjectName(std::string_view projectName, const Path& path);

	private:
		inline static std::shared_ptr<Project> s_ActiveProject;
		ProjectSettings m_ProjectSettings;

	private:
		static constexpr std::string_view TEMPLATE_DIRECTORY = "Resources/ProjectTemplate";
		static constexpr std::string_view TEMPLATE_PREMAKE = "premake5.lua";
		static constexpr std::string_view TEMPLATE_PROJ_GEN = "Win-GenProjects.bat";
		static constexpr std::string_view TEMPLATE_SCRIPT_FILE = "Sample.cs";
	};
}