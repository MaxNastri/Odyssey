#pragma once
#include "DockableWindow.h"
#include "ResourceHandle.h"
#include "GameObject.h"

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
		SceneViewWindow();
		void Destroy();

	public:
		virtual void Update() override;
		virtual void Draw() override;
		virtual void OnWindowResize() override;

	public:
		std::shared_ptr<OpaquePass> GetRenderPass() { return m_SceneViewPass; }
		void SetSelectedGameObject(GameObject* gameObject) { m_SelectedObject = gameObject; }
		void OnSceneChanged();

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();
		void RenderGizmos();
		void UpdateCameraController();
		void UpdateGizmosInput();

	private: // Camera stuff
		GameObject* m_GameObject = nullptr;
		Transform* m_CameraTransform = nullptr;
		Camera* m_Camera = nullptr;

		bool m_CameraControllerInUse = false;

	private: // Rendering stuff
		std::shared_ptr<OpaquePass> m_SceneViewPass;
		uint64_t m_RenderTextureID;
		ResourceHandle<VulkanRenderTexture> m_RenderTexture;
		ResourceHandle<VulkanRenderTexture> m_DepthRT;
		ResourceHandle<VulkanTextureSampler> m_RTSampler;

	private: // Gizmos
		GameObject* m_SelectedObject = nullptr;
		uint32_t op = 7;
	};
}