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
		void SetSelectedIndex(uint32_t selected) { m_SelectedObject = GameObject(selected); }

	private:
		void CreateRenderTexture();
		void DestroyRenderTexture();
		void RenderGizmos();
		void UpdateCameraController();
		void UpdateGizmosInput();

	private: // Camera stuff
		GameObject m_GameObject;
		Transform* m_CameraTransform;
		Camera* m_Camera;

		bool m_CameraControllerInUse = false;

	private: // Rendering stuff
		std::shared_ptr<OpaquePass> m_SceneViewPass;
		uint64_t m_RenderTextureID;
		ResourceHandle<VulkanRenderTexture> m_RenderTexture;
		ResourceHandle<VulkanTextureSampler> m_RTSampler;

	private: // Gizmos
		GameObject m_SelectedObject;
		uint32_t op = 7;
	};
}