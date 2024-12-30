#pragma once
#include "Ref.h"
#include "RawBuffer.h"

namespace Odyssey
{
	class ClipBoard
	{
	public:
		static void Copy(std::string_view context, const void* data, size_t size)
		{
			UserContext = context;
			RawBuffer::Copy(UserData, data, size);
		}

		static RawBuffer& Paste() { return UserData; }
		static std::string_view GetContext() { return UserContext; }

		inline static RawBuffer UserData;
		inline static std::string UserContext;
	};

	class Preferences
	{
	public:
		Preferences() = default;
		Preferences(const Path& path);

	private:
		void Load(const Path& path);
		void Save();
		void SaveTo(const Path& path);

	public:
		static void LoadPreferences(const Path& path);
		static void SavePreferences();

	public:
		static mat4 GetSceneView() { return s_Instance->m_SceneView; }
		static const Path& GetStartupProject() { return s_Instance->m_StartupProject; }
		static const Path& GetEditorRegistry() { return s_Instance->m_EditorRegistry; }
		static const GUID& GetDefaultSprite() { return s_Instance->m_DefaultSprite; }
		static const GUID& GetFolderIcon() { return s_Instance->m_FolderIcon; }
		static const GUID& GetScriptIcon() { return s_Instance->m_ScriptIcon; }
		static const GUID& GetMaterialIcon() { return s_Instance->m_MaterialIcon; }
		static const GUID& GetMeshIcon() { return s_Instance->m_MeshIcon; }
		static const GUID& GetAssetIcon() { return s_Instance->m_AssetIcon; }
		static const std::vector<std::string>& GetAssetExtensions() { return s_Instance->m_AssetExtensions; }
		static const std::vector<std::string>& GetSourceExtensions() { return s_Instance->m_SourceExtensions; }
		static const std::map<std::string, std::string>& GetSourceExtensionsMap() { return s_Instance->m_SourceExtensionsToType; }

	public:
		static void SetSceneView(mat4 sceneView);
	
	private: // Singleton
		inline static Ref<Preferences> s_Instance;

	private:
		Path m_Path;

	private:
		std::vector<std::string> m_AssetExtensions;
		std::vector<std::string> m_SourceExtensions;
		std::map<std::string, std::string> m_SourceExtensionsToType;

	private:
		mat4 m_SceneView = mat4(1.0f);

	private: // Icons
		GUID m_FolderIcon;
		GUID m_ScriptIcon;
		GUID m_MaterialIcon;
		GUID m_MeshIcon;
		GUID m_AssetIcon;

	private: // Pathing
		Path m_StartupProject;
		Path m_EditorRegistry;

	private: // Debug
		uint64_t m_DebugRendererVertices;

	private: // Default assets
		GUID m_DefaultSprite;
	};
}