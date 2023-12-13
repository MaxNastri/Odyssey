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

	private: // Pathing
		bool m_UpdatePaths = true;
		std::filesystem::path m_CurrentPath;
		std::vector<std::pair<std::filesystem::path, bool>> m_PathsToDisplay;

	private: // Context menu
		bool m_ContextMenuOpen = false;

	private: // Statics
		inline static std::filesystem::path s_AssetsPath = "Assets";
	};
}