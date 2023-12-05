#pragma once
#include "GUIElement.h"
#include "glm.h"
#include "FileManager.h"
#include <filesystem>

namespace Odyssey
{
	class ContentBrowserWindow : public GUIElement
	{
	public:
		ContentBrowserWindow();
		virtual void Update() override;
		virtual void Draw() override;
		void Destroy();

	public:
		void UpdatePaths();

	private:
		bool m_UpdatePaths = true;
		std::filesystem::path m_CurrentPath;
		std::vector<std::pair<std::filesystem::path, bool>> m_PathsToDisplay;

	private: // Window stuff
		bool m_Open = true;
		glm::vec2 m_WindowPos;
		glm::vec2 m_WindowSize;
		glm::vec2 m_WindowMin;
		glm::vec2 m_WindowMax;
	};
}