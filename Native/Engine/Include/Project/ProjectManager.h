#pragma once
#include "ProjectSettings.h"

namespace Odyssey
{
	class ProjectManager
	{
	public:
		static void CreateNewProject(const std::string& projectName, const std::filesystem::path& projectDirectory);
		static void LoadProject(const std::filesystem::path& projectDirectory);

	public:
		static const std::filesystem::path& GetProjectDirectory() { return m_Settings.GetProjectDirectory(); }
		static const std::filesystem::path& GetAssetsDirectory() { return m_Settings.GetAssetsDirectory(); }
		static const std::filesystem::path& GetCacheDirectory() { return m_Settings.GetCacheDirectory(); }
		static const std::filesystem::path& GetLogsDirectory() { return m_Settings.GetLogsDirectory(); }
		static const std::filesystem::path& GetUserScriptsProject() { return m_Settings.GetScriptsProjectPath(); }
		static const std::filesystem::path& GetTempDirectory() { return m_Settings.GetTempDirectory(); }

	private:
		inline static ProjectSettings m_Settings;

	private:
		static constexpr std::string_view TEMPLATE_DIRECTORY = "Resources/ProjectTemplate";
		static constexpr std::string_view TEMPLATE_PREMAKE = "premake5.lua";
		static constexpr std::string_view TEMPLATE_PROJ_GEN = "Win-GenProjects.bat";
	};
}