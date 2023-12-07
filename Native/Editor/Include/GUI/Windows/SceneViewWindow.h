#pragma once
#include <GUIElement.h>
#include "glm.h"
#include "ResourceHandle.h"
#include "Camera.h"

namespace Odyssey
{
	class VulkanTexture;
	class OpaquePass;

	class SceneViewWindow : public GUIElement
	{
	public:
		SceneViewWindow();
		virtual void Update() override;
		virtual void Draw() override;
		void Destroy();

	public:
		std::shared_ptr<OpaquePass> GetRenderPass() { return m_SceneViewPass; }

		void SetSelectedIndex(uint32_t selected) { m_SelectedObject = selected; }

	private:
		void UpdateWindowProperties();
		void CreateRenderTexture(uint32_t index);
		void DestroyRenderTexture(uint32_t index);
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
		std::vector<uint64_t> m_RenderTextureID;
		std::vector<ResourceHandle<VulkanTexture>> m_RenderTexture;

	private: // Window stuff
		bool open = true;
		uint32_t m_SelectedObject;
		glm::vec2 m_WindowPos;
		glm::vec2 m_WindowSize;
		glm::vec2 m_WindowMin;
		glm::vec2 m_WindowMax;
		glm::vec2 m_FramePadding;
		bool m_CursorInContentRegion = false;
		bool m_WindowResized = false;
		uint32_t op = 7;
	};
}