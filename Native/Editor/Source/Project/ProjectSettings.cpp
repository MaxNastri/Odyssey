#include "ProjectSettings.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	ProjectSettings::ProjectSettings(const std::string& projectName, const std::filesystem::path& projectDirectory)
	{
		ProjectName = projectName;
		m_Path = projectDirectory / projectName / "ProjectSettings.asset";

		Save();
	}

	ProjectSettings::ProjectSettings(const std::filesystem::path& settingsPath)
	{
		if (std::filesystem::is_directory(settingsPath))
			m_Path = settingsPath / "ProjectSetings.asset";
		else
			m_Path = settingsPath;
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

		root.WriteData("ProjectName", ProjectName);
		serializer.WriteToDisk(m_Path);
	}

	void ProjectSettings::Load()
	{
		if (m_Path.empty())
			return;

		if (AssetDeserializer file = AssetDeserializer(m_Path))
		{
			SerializationNode root = file.GetRoot();
			root.ReadData("ProjectName", ProjectName);
		}
	}
}