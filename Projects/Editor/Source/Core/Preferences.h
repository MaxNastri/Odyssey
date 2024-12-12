#pragma once
#include "Ref.h"
#include "RawBuffer.h"

namespace Odyssey
{
	class ClipBoard
	{
	public:
		static void Copy(const void* data, size_t size)
		{
			RawBuffer::Copy(UserData, data, size);
		}

		static RawBuffer& Paste() { return UserData; }

		inline static RawBuffer UserData;
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
		static const Path& GetStartupProject() { return s_Instance->m_StartupProject; }
		static const Path& GetEditorRegistry() { return s_Instance->m_EditorRegistry; }
		static const GUID& GetFolderIcon() { return s_Instance->m_FolderIcon; }
		static const GUID& GetScriptIcon() { return s_Instance->m_ScriptIcon; }
		static const GUID& GetMaterialIcon() { return s_Instance->m_MaterialIcon; }
		static const GUID& GetMeshIcon() { return s_Instance->m_MeshIcon; }
		static const GUID& GetAssetIcon() { return s_Instance->m_AssetIcon; }
		static const std::vector<std::string>& GetAssetExtensions() { return s_Instance->m_AssetExtensions; }
		static const std::vector<std::string>& GetSourceExtensions() { return s_Instance->m_SourceExtensions; }
		static const std::map<std::string, std::string>& GetSourceExtensionsMap() { return s_Instance->m_SourceExtensionsToType; }
		
	private: // Singleton
		inline static Ref<Preferences> s_Instance;

	private:
		Path m_Path;

	private:
		std::vector<std::string> m_AssetExtensions;
		std::vector<std::string> m_SourceExtensions;
		std::map<std::string, std::string> m_SourceExtensionsToType;

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
	};
}