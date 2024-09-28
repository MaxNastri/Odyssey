#pragma once

namespace Odyssey
{
	class ProjectSettings
	{
	public:
		ProjectSettings() = default;
		ProjectSettings(const std::string& projectName, const Path& projectDirectory);
		ProjectSettings(const Path& settingsPath);

	public:
		void Save();
		void Load();

	public:
		const Path& GetPath() { return m_Path; }
		const Path& GetProjectDirectory() { return m_ProjectDirectory; }

	public:
		const Path& GetAssetsDirectory() { return m_FullAssetsDirectory; }
		const Path& GetCacheDirectory() { return m_FullCacheDirectory; }
		const Path& GetTempDirectory() { return m_FullTempDirectory; }
		const Path& GetLogsDirectory() { return m_FullLogsDirectory; }
		const Path& GetScriptsDirectory() { return m_FullScriptsDirectory; }
		const Path& GetCodeDirectory() { return m_FullCodeDirectory; }
		const Path& GetScriptsProjectPath() { return m_FullScriptsProjectPath; }
		const Path& GetAssetRegistryPath() { return m_FullAssetRegistryPath; }

	private: // Serialized
		std::string m_ProjectName;
		Path m_AssetsDirectory;
		Path m_CacheDirectory;
		Path m_TempDirectory;
		Path m_LogsDirectory;
		Path m_ScriptsDirectory;
		Path m_CodeDirectory;
		Path m_ScriptsProjectPath;
		Path m_AssetRegistryPath;

	private: // Generated
		Path m_FullAssetsDirectory;
		Path m_FullCacheDirectory;
		Path m_FullTempDirectory;
		Path m_FullLogsDirectory;
		Path m_FullScriptsDirectory;
		Path m_FullCodeDirectory;
		Path m_FullScriptsProjectPath;
		Path m_FullAssetRegistryPath;

	private:
		Path m_Path;
		Path m_ProjectDirectory;
	};
}