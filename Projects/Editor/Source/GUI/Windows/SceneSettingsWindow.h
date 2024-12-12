#pragma once
#include "Ref.h"
#include "DockableWindow.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class Scene;
	struct IEventListener;
	struct SceneLoadedEvent;

	class SceneSettingsWindow : public DockableWindow
	{
	public:
		SceneSettingsWindow(size_t windowID);

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	private:
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		AssetFieldDrawer m_SkyboxDrawer;
		ColorPicker m_AmbientColorPicker;
		Vector3Drawer m_SceneCenterDrawer;
		FloatDrawer m_SceneRadiusDrawer;

	private:
		Scene* m_Scene;
		Ref<IEventListener> m_SceneLoadedListener;
	};
}