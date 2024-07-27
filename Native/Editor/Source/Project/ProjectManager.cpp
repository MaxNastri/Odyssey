#include "ProjectManager.h"
#include "Logger.h"

namespace Odyssey
{
	void ProjectManager::CreateNewProject(ProjectSettings projectSettings)
	{
		SetProject(projectSettings);
		GeneratePaths();

		if (!std::filesystem::exists(m_AssetsDirectory))
			std::filesystem::create_directories(m_AssetsDirectory);

		if (!std::filesystem::exists(m_CacheDirectory))
			std::filesystem::create_directories(m_CacheDirectory);

		if (!std::filesystem::exists(m_LogsDirectory))
			std::filesystem::create_directories(m_LogsDirectory);

		m_Settings.Save();
	}

	void ProjectManager::SetProject(ProjectSettings projectSettings)
	{
		m_Settings = projectSettings;
		GeneratePaths();
	}

	void ProjectManager::GeneratePaths()
	{
		// Get the path of the project settings
		auto settingsPath = m_Settings.GetPath();

		// Build the paths for key directories based on the project settings path
		m_ProjectDirectory = settingsPath.parent_path();
		m_AssetsDirectory = m_ProjectDirectory / ASSETS_DIRECTORY;
		m_CacheDirectory = m_ProjectDirectory / CACHE_DIRECTORY;
		m_LogsDirectory = m_ProjectDirectory / LOGS_DIRECTORY;

		if (!std::filesystem::exists(m_AssetsDirectory))
			Logger::LogError("[ProjectManager] Could not locate asset directory: " + m_AssetsDirectory.string());

		if (!std::filesystem::exists(m_CacheDirectory))
			Logger::LogError("[ProjectManager] Could not locate cache directory: " + m_CacheDirectory.string());

		if (!std::filesystem::exists(m_LogsDirectory))
			Logger::LogError("[ProjectManager] Could not locate logs directory: " + m_LogsDirectory.string());

	}
}