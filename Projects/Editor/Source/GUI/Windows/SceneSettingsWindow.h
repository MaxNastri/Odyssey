#pragma once
#include "DockableWindow.h"
#include "AssetFieldDrawer.h"
#include "ColorDrawer.h"

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
		virtual void Draw() override;
		virtual void OnWindowClose() override;

	private:
		void OnSceneLoaded(SceneLoadedEvent* event);
		void OnSkyboxChanged(GUID skyboxGUID);
		void OnAmbientColorChanged(glm::vec3 color);

	private:
		AssetFieldDrawer m_SkyboxDrawer;
		Color3Drawer m_AmbientColorDrawer;

	private:
		Scene* m_Scene;
		std::shared_ptr<IEventListener> m_SceneLoadedListener;
	};
}