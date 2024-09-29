#include "ProjectSettings.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	ProjectSettings::ProjectSettings(const std::string& projectName, const Path& projectDirectory)
	{
		m_ProjectName = projectName;
		m_Path = projectDirectory / projectName / "ProjectSettings.osettings";
		m_ProjectDirectory = projectDirectory;
	}

	ProjectSettings::ProjectSettings(const Path& settingsPath)
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

		root.WriteData("ProjectName", m_ProjectName);
		root.WriteData("AssetsDirectory", m_AssetsDirectory.string());
		root.WriteData("CacheDirectory", m_CacheDirectory.string());
		root.WriteData("TempDirectory", m_TempDirectory.string());
		root.WriteData("LogsDirectory", m_LogsDirectory.string());
		root.WriteData("ScriptsDirectory", m_ScriptsDirectory.string());
		root.WriteData("CodeDirectory", m_CodeDirectory.string());
		root.WriteData("ScriptsProjectPath", m_ScriptsProjectPath.string());
		root.WriteData("AssetRegistryPath", m_AssetRegistryPath.string());
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
			std::string assetRegistryPath;

			root.ReadData("ProjectName", m_ProjectName);
			root.ReadData("AssetsDirectory", assetsDirectory);
			root.ReadData("CacheDirectory", cacheDirectory);
			root.ReadData("TempDirectory", tempDirectory);
			root.ReadData("LogsDirectory", logsDirectory);
			root.ReadData("ScriptsDirectory", scriptsDirectory);
			root.ReadData("CodeDirectory", codeDirectory);
			root.ReadData("ScriptsProjectPath", scriptsProjectPath);
			root.ReadData("AssetRegistryPath", assetRegistryPath);

			// Convert them back into paths
			m_AssetsDirectory = assetsDirectory;
			m_CacheDirectory = cacheDirectory;
			m_TempDirectory = tempDirectory;
			m_LogsDirectory = logsDirectory;
			m_ScriptsDirectory = scriptsDirectory;
			m_CodeDirectory = codeDirectory;
			m_ScriptsProjectPath = scriptsProjectPath;
			m_AssetRegistryPath = assetRegistryPath;

			// Generate paths including the project directory
			m_FullAssetsDirectory = m_ProjectDirectory / m_AssetsDirectory;
			m_FullCacheDirectory = m_ProjectDirectory / m_CacheDirectory;
			m_FullTempDirectory = m_ProjectDirectory / m_TempDirectory;
			m_FullLogsDirectory = m_ProjectDirectory / m_LogsDirectory;
			m_FullScriptsDirectory = m_ProjectDirectory / m_ScriptsDirectory;
			m_FullCodeDirectory = m_ProjectDirectory / m_CodeDirectory;
			m_FullScriptsProjectPath = m_ProjectDirectory / m_ScriptsProjectPath;
			m_FullAssetRegistryPath = m_ProjectDirectory / m_AssetRegistryPath;

			std::filesystem::create_directories(m_FullAssetsDirectory);
			std::filesystem::create_directories(m_FullCacheDirectory);
			std::filesystem::create_directories(m_FullTempDirectory);
			std::filesystem::create_directories(m_FullLogsDirectory);
			std::filesystem::create_directories(m_FullScriptsDirectory);
			std::filesystem::create_directories(m_FullCodeDirectory);
		}
	}
}