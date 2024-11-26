#pragma once
#include "Ref.h"
#include "DockableWindow.h"
#include "FileTracker.h"
#include "EditorWidgets.h"

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

	private:
		Path GetUniquePath(const Path& filename, const Path& extension);

	private: // Pathing
		bool m_UpdatePaths = true;
		Path m_AssetsPath;
		Path m_CurrentPath;
		std::vector<Path> m_FoldersToDisplay;
		std::vector<Path> m_FilesToDisplay;
		std::unique_ptr<FileTracker> m_FileTracker;
		Ref<SelectableInput> m_Input;

	private: // Context menu
		bool m_ContextMenuOpen = false;
	};
}