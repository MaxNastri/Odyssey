#include "ProjectSettings.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	ProjectSettings::ProjectSettings(const std::string& projectName, const std::filesystem::path& projectDirectory)
	{
		m_ProjectName = projectName;
		m_Path = projectDirectory / projectName / "ProjectSettings.asset";
		m_ProjectDirectory = projectDirectory;
	}

	ProjectSettings::ProjectSettings(const std::filesystem::path& settingsPath)
	{
		m_Path = settingsPath;
		m_ProjectDirectory = m_Path.parent_path();
		Load();
	}

	void ProjectSettings::Save()
	{
		if (m_Path.empty())
			return;

		auto parent = m_Path.parent_path();
		if (!std::filesystem::exists(parent))
			std::filesystem::create_directories(parent);

		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		root.WriteData("m_ProjectName", m_ProjectName);
		root.WriteData("m_AssetsDirectory", m_AssetsDirectory.string());
		root.WriteData("m_CacheDirectory", m_CacheDirectory.string());
		root.WriteData("m_TempDirectory", m_TempDirectory.string());
		root.WriteData("m_LogsDirectory", m_LogsDirectory.string());
		root.WriteData("m_ScriptsDirectory", m_ScriptsDirectory.string());
		root.WriteData("m_CodeDirectory", m_CodeDirectory.string());
		root.WriteData("m_ScriptsProjectPath", m_ScriptsProjectPath.string());
		serializer.WriteToDisk(m_Path);
	}

	void ProjectSettings::Load()
	{
		if (m_Path.empty())
			return;

		if (AssetDeserializer file = AssetDeserializer(m_Path))
		{
			SerializationNode root = file.GetRoot();

			// ryml doesn't like serializating paths
			std::string assetsDirectory;
			std::string cacheDirectory;
			std::string tempDirectory;
			std::string logsDirectory;
			std::string scriptsDirectory;
			std::string codeDirectory;
			std::string scriptsProjectPath;

			root.ReadData("m_ProjectName", m_ProjectName);
			root.ReadData("m_AssetsDirectory", assetsDirectory);
			root.ReadData("m_CacheDirectory", cacheDirectory);
			root.ReadData("m_TempDirectory", tempDirectory);
			root.ReadData("m_LogsDirectory", logsDirectory);
			root.ReadData("m_ScriptsDirectory", scriptsDirectory);
			root.ReadData("m_CodeDirectory", codeDirectory);
			root.ReadData("m_ScriptsProjectPath", scriptsProjectPath);

			// Convert them back into paths
			m_AssetsDirectory = assetsDirectory;
			m_CacheDirectory = cacheDirectory;
			m_TempDirectory = tempDirectory;
			m_LogsDirectory = logsDirectory;
			m_ScriptsDirectory = scriptsDirectory;
			m_CodeDirectory = codeDirectory;
			m_ScriptsProjectPath = scriptsProjectPath;

			// Generate paths including the project directory
			m_FullAssetsDirectory = m_ProjectDirectory / m_AssetsDirectory;
			m_FullCacheDirectory = m_ProjectDirectory / m_CacheDirectory;
			m_FullTempDirectory = m_ProjectDirectory / m_TempDirectory;
			m_FullLogsDirectory = m_ProjectDirectory / m_LogsDirectory;
			m_FullScriptsDirectory = m_ProjectDirectory / m_ScriptsDirectory;
			m_FullCodeDirectory = m_ProjectDirectory / m_CodeDirectory;
			m_FullScriptsProjectPath = m_ProjectDirectory / m_ScriptsProjectPath;

			std::filesystem::create_directories(m_FullAssetsDirectory);
			std::filesystem::create_directories(m_FullCacheDirectory);
			std::filesystem::create_directories(m_FullTempDirectory);
			std::filesystem::create_directories(m_FullLogsDirectory);
			std::filesystem::create_directories(m_FullScriptsDirectory);
			std::filesystem::create_directories(m_FullCodeDirectory);
		}
	}
}