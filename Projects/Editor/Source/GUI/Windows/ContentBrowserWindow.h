#pragma once
#include "Ref.h"
#include "DockableWindow.h"
#include "FileTracker.h"
#include "EditorWidgets.h"
#include "Texture2D.h"

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
		void OnFileAction(const Path& oldPath, const Path& newPath, FileActionType fileAction);

	private:
		Path GetUniquePath(const Path& filename, const Path& extension);

	private: // Pathing
		bool m_UpdatePaths = true;
		Path m_AssetsPath;
		Path m_CurrentPath;
		std::vector<Path> m_FoldersToDisplay;
		std::vector<Path> m_FilesToDisplay;
		std::unique_ptr<FileTracker> m_FileTracker;
		std::vector<SelectableInput> m_FolderDrawers;
		std::vector<SelectableInput> m_AssetDrawers;

	private: // Context menu
		bool m_ContextMenuOpen = false;
	};
}