#pragma once
#include "ContentBrowserWindow.h"
#include "EditorActionsBar.h"
#include "EditorEnums.h"
#include "EditorMenuBar.h"
#include "GameViewWindow.h"
#include "InspectorWindow.h"
#include "Ref.h"
#include "SceneHierarchyWindow.h"
#include "SceneViewWindow.h"

namespace Odyssey
{
	struct OnGUIRenderEvent;
	struct SceneLoadedEvent;
	class ImguiPass;

	class GUIManager
	{
	public:
		static void Initialize();

	public:
		static void CreateInspectorWindow();

	public:
		template<typename T>
		static void CreateDockableWindow()
		{
			static_assert(std::is_base_of<DockableWindow, T>::value, "T is not a dervied class of DockableWindow.");

			std::type_index windowType = typeid(T);

			size_t windowID = 0;

			// Check if we have an available ID
			if (!s_WindowIDs[windowType].AvailableIDs.empty())
			{
				windowID = s_WindowIDs[windowType].AvailableIDs.top();
				s_WindowIDs[windowType].AvailableIDs.pop();
			}
			else
			{
				// Use the next window ID
				windowID = s_WindowIDs[windowType].NextID;
				s_WindowIDs[windowType].NextID++;
			}

			// Create and store the window
			s_Windows.emplace_back(new T(windowID));
		}

		template<typename T>
		static void DestroyDockableWindow(T* destroyWindow)
		{
			std::type_index windowType = typeid(T);

			for (size_t i = 0; i < s_Windows.size(); i++)
			{
				if (s_Windows[i].Get() == destroyWindow)
				{
					s_WindowIDs[windowType].AvailableIDs.push(destroyWindow->GetID());
					s_Windows.erase(s_Windows.begin() + i);
					break;
				}
			}
		}

	public:
		static void Update();
		static void DrawGUI();

	private:
		static void SetDarkThemeColors();

	private:
		inline static EditorMenuBar s_MenuBar;
		inline static EditorActionsBar s_ActionsBar;

		struct WindowID
		{
		public:
			size_t NextID;
			std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> AvailableIDs;
		};

		// Windows
		inline static std::vector<Ref<DockableWindow>> s_Windows;
		inline static std::unordered_map<std::type_index, WindowID> s_WindowIDs;
		inline static int32_t selectedObject = -1;
	};
}