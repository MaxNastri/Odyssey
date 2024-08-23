#pragma once

namespace Odyssey
{
	class ProjectSettings
	{
	public:
		ProjectSettings() = default;
		ProjectSettings(const std::string& projectName, const std::filesystem::path& projectDirectory);
		ProjectSettings(const std::filesystem::path& settingsPath);

	public:
		void Save();
		void Load();

	public:
		const std::filesystem::path& GetPath() { return m_Path; }
		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }

	public:
		const std::filesystem::path& GetAssetsDirectory() { return m_FullAssetsDirectory; }
		const std::filesystem::path& GetCacheDirectory() { return m_FullCacheDirectory; }
		const std::filesystem::path& GetTempDirectory() { return m_FullTempDirectory; }
		const std::filesystem::path& GetLogsDirectory() { return m_FullLogsDirectory; }
		const std::filesystem::path& GetScriptsDirectory() { return m_FullScriptsDirectory; }
		const std::filesystem::path& GetCodeDirectory() { return m_FullCodeDirectory; }
		const std::filesystem::path& GetScriptsProjectPath() { return m_FullScriptsProjectPath; }

	private: // Serialized
		std::string m_ProjectName;
		std::filesystem::path m_AssetsDirectory;
		std::filesystem::path m_CacheDirectory;
		std::filesystem::path m_TempDirectory;
		std::filesystem::path m_LogsDirectory;
		std::filesystem::path m_ScriptsDirectory;
		std::filesystem::path m_CodeDirectory;
		std::filesystem::path m_ScriptsProjectPath;

	private: // Generated
		std::filesystem::path m_FullAssetsDirectory;
		std::filesystem::path m_FullCacheDirectory;
		std::filesystem::path m_FullTempDirectory;
		std::filesystem::path m_FullLogsDirectory;
		std::filesystem::path m_FullScriptsDirectory;
		std::filesystem::path m_FullCodeDirectory;
		std::filesystem::path m_FullScriptsProjectPath;

	private:
		std::filesystem::path m_Path;
		std::filesystem::path m_ProjectDirectory;
	};
}