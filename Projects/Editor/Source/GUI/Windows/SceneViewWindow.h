#pragma once
#include "DockableWindow.h"
#include "Events.h"
#include "EventSystem.h"
#include "GameObject.h"
#include "Ref.h"
#include "Resource.h"

namespace Odyssey
{
	class Camera;
	class RenderObjectsPass;
	class Transform;
	class VulkanTextureSampler;
	struct GUISelectionChangedEvent;
	struct PlaymodeStateChangedEvent;

	class SceneViewWindow : public DockableWindow
	{
	public:
		SceneViewWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual bool Draw() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowClose() override;

	private:
		void OnSceneLoaded(SceneLoadedEvent* eventData);
		void OnGUISelectionChanged(GUISelectionChangedEvent* eventData);
		void OnPlaymodeStateChanged(PlaymodeStateChangedEvent* eventData);

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();
		void RenderGizmos();
		void UpdateCameraController();
		void UpdateGizmosInput();

	private: // Event listener
		Ref<IEventListener> m_SceneLoadedListener;
		Ref<IEventListener> m_GUISelectionListener;
		Ref<IEventListener> m_PlaymodeStateChangedListener;

	private: // Camera stuff
		GameObject m_GameObject;
		bool m_CameraControllerInUse = false;
		bool m_AllowInput = true;

	private: // Rendering stuff
		Ref<RenderObjectsPass> m_SceneViewPass;
		uint64_t m_RenderTextureID;
		ResourceID m_RenderTarget;
		ResourceID m_RTSampler;

	private: // Gizmos
		Scene* m_ActiveScene;
		GameObject m_SelectedGO;
		uint32_t op = 7;

	public:
		// TODO: Move these to editor settings file
		inline static bool AllowFlip = false;
		inline static bool IsLocal = true;
	};
}