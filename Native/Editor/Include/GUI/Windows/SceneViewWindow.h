#pragma once
#include "DockableWindow.h"
#include "ResourceHandle.h"
#include "GameObject.h"

namespace Odyssey
{
	class Camera;
	class Transform;
	class VulkanTexture;
	class OpaquePass;

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
		void SetSelectedIndex(uint32_t selected) { m_SelectedObject = selected; }

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
		ResourceHandle<VulkanTexture> m_RenderTexture;

	private: // Gizmos
		uint32_t m_SelectedObject;
		uint32_t op = 7;
	};
}