#pragma once
#include "DockableWindow.h"
#include "Resource.h"
#include "GameObject.h"
#include "Events.h"
#include "EventSystem.h"

namespace Odyssey
{
	class Camera;
	class OpaquePass;
	class Transform;
	class VulkanRenderTexture;
	class VulkanTextureSampler;

	class SceneViewWindow : public DockableWindow
	{
	public:
		SceneViewWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual void Draw() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowClose() override;

	public:
		std::shared_ptr<OpaquePass> GetRenderPass() { return m_SceneViewPass; }
		void SetSelectedGameObject(GameObject* gameObject) { m_SelectedObject = gameObject; }
		void OnSceneLoaded(SceneLoadedEvent* event);

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();
		void RenderGizmos();
		void UpdateCameraController();
		void UpdateGizmosInput();

	private: // Event listener
		std::shared_ptr<IEventListener> m_SceneLoadedListener;

	private: // Camera stuff
		GameObject m_GameObject;
		bool m_CameraControllerInUse = false;

	private: // Rendering stuff
		std::shared_ptr<OpaquePass> m_SceneViewPass;
		uint64_t m_RenderTextureID;
		ResourceID m_ColorRT;
		ResourceID m_DepthRT;
		ResourceID m_RTSampler;

	private: // Gizmos
		GameObject* m_SelectedObject = nullptr;
		uint32_t op = 7;
	};
}