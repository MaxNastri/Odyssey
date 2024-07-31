#pragma once
#include "DockableWindow.h"
#include "glm.h"
#include "FileTracker.h"

namespace Odyssey
{
	class ContentBrowserWindow : public DockableWindow
	{
	public:
		ContentBrowserWindow();
		void Destroy();

	public:
		virtual void Draw() override;

	public:
		void UpdatePaths();

	private:
		void HandleContextMenu();
		void DrawSceneAsset(const std::filesystem::path& assetPath);
		void DrawSourceAsset(const std::filesystem::path& sourcePath);
		void DrawAsset(const std::filesystem::path& assetPath);
		void OnFileAction(const std::filesystem::path& filePath, FileActionType fileAction);

	private: // Pathing
		bool m_UpdatePaths = true;
		std::filesystem::path m_AssetsPath;
		std::filesystem::path m_CurrentPath;
		std::vector<std::filesystem::path> m_FoldersToDisplay;
		std::vector<std::filesystem::path> m_FilesToDisplay;
		std::unique_ptr<FileTracker> m_FileTracker;

	private: // Context menu
		bool m_ContextMenuOpen = false;
	};
}