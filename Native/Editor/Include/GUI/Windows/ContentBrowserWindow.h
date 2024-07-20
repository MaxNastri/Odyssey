#pragma once
#include "DockableWindow.h"
#include "glm.h"
#include "FileManager.h"
#include <filesystem>

namespace Odyssey
{
	class ContentBrowserWindow : public DockableWindow
	{
	public:
		ContentBrowserWindow();
		void Destroy();

	public:
		virtual void Update() override;
		virtual void Draw() override;

	public:
		void UpdatePaths();

	private:
		void HandleContextMenu();
		void DrawSceneAsset(const std::filesystem::path& assetPath);
		void DrawSourceAsset(const std::filesystem::path& sourcePath);
		void DrawAsset(const std::filesystem::path& assetPath);

	private: // Pathing
		bool m_UpdatePaths = true;
		std::filesystem::path m_CurrentPath;
		std::vector<std::filesystem::path> m_FoldersToDisplay;
		std::vector<std::filesystem::path> m_FilesToDisplay;

	private: // Context menu
		bool m_ContextMenuOpen = false;

	private: // Statics
		inline static std::filesystem::path s_AssetsPath = "Assets";
	};
}