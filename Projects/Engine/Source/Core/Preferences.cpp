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

			root.ReadData("Scene View", m_SceneView);
			root.ReadData("Startup Project", startupProject);
			root.ReadData("Editor Registry", registryPath);
			root.ReadData("Default Sprite", m_DefaultSprite.Ref());
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

		root.WriteData("Scene View", m_SceneView);
		root.WriteData("Startup Project", m_StartupProject.string());
		root.WriteData("Editor Registry", m_EditorRegistry.string());
		root.WriteData("Default Sprite", m_DefaultSprite.CRef());
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

	void Preferences::SetSceneView(mat4 sceneView)
	{
		s_Instance->m_SceneView = sceneView;
		s_Instance->Save();
	}
}