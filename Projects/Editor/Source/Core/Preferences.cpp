#include "Preferences.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	Preferences::Preferences(const Path& path)
	{
		m_Path = path;
		Load(path);
	}

	void Preferences::Load(const Path& path)
	{
		AssetDeserializer deserializer(path);

		if (deserializer.IsValid())
		{
			SerializationNode& root = deserializer.GetRoot();

			std::string registryPath, startupProject;

			root.ReadData("Startup Project", startupProject);
			root.ReadData("Editor Registry", registryPath);
			root.ReadData("Folder Icon", m_FolderIcon.Ref());
			root.ReadData("Script Icon", m_ScriptIcon.Ref());
			root.ReadData("Asset Icon", m_AssetIcon.Ref());
			root.ReadData("Material Icon", m_MaterialIcon.Ref());
			root.ReadData("Mesh Icon", m_MeshIcon.Ref());
			root.ReadData("Asset Extensions", m_AssetExtensions);
			root.ReadData("Source Extensions", m_SourceExtensionsToType);

			m_StartupProject = startupProject;
			m_EditorRegistry = registryPath;

			for (auto& [extension, sourceType] : m_SourceExtensionsToType)
			{
				m_SourceExtensions.emplace_back(extension);
			}
		}
	}

	void Preferences::Save()
	{
		SaveTo(m_Path);
	}

	void Preferences::SaveTo(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode& root = serializer.GetRoot();

		root.WriteData("Startup Project", m_StartupProject.string());
		root.WriteData("Editor Registry", m_EditorRegistry.string());
		root.WriteData("Folder Icon", m_FolderIcon.CRef());
		root.WriteData("Script Icon", m_ScriptIcon.CRef());
		root.WriteData("Asset Icon", m_AssetIcon.CRef());
		root.WriteData("Material Icon", m_MaterialIcon.CRef());
		root.WriteData("Mesh Icon", m_MeshIcon.CRef());
		root.WriteData("Asset Extensions", m_AssetExtensions);
		root.WriteData("Source Extensions", m_SourceExtensionsToType);

		serializer.WriteToDisk(path);
	}

	void Preferences::LoadPreferences(const Path& path)
	{
		s_Instance = new Preferences(path);
	}

	void Preferences::SavePreferences()
	{
		if (s_Instance)
			s_Instance->Save();
	}
}