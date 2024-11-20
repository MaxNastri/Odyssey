#pragma once
#include "DockableWindow.h"
#include "FileTracker.h"

namespace Odyssey
{
	class ContentBrowserWindow : public DockableWindow
	{
	public:
		ContentBrowserWindow(size_t windowID);
		void Destroy();

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	public:
		void UpdatePaths();

	private:
		void HandleContextMenu();
		void DrawSceneAsset(const Path& assetPath);
		void DrawSourceAsset(const Path& sourcePath);
		void DrawAsset(const Path& assetPath);
		void OnFileAction(const Path& filePath, FileActionType fileAction);

	private: // Pathing
		bool m_UpdatePaths = true;
		Path m_AssetsPath;
		Path m_CurrentPath;
		std::vector<Path> m_FoldersToDisplay;
		std::vector<Path> m_FilesToDisplay;
		std::unique_ptr<FileTracker> m_FileTracker;

	private: // Context menu
		bool m_ContextMenuOpen = false;
	};
}