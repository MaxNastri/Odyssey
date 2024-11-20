#pragma once
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
		void OnSkyboxChanged(GUID skyboxGUID);
		void OnAmbientColorChanged(glm::vec3 color);

	private:
		AssetFieldDrawer m_SkyboxDrawer;
		ColorPicker m_AmbientColorPicker;

	private:
		Scene* m_Scene;
		std::shared_ptr<IEventListener> m_SceneLoadedListener;
	};
}