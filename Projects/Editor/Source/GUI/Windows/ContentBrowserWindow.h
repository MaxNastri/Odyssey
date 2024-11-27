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

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_ScriptIcon;
		uint64_t m_FolderIconHandle = 0;
		uint64_t m_ScriptIconHandle = 0;

	private: // Context menu
		bool m_ContextMenuOpen = false;
		inline static const GUID& Folder_Icon_GUID = 213981209753892768;
		inline static const GUID& Script_Icon_GUID = 128378120928579883;
	};
}