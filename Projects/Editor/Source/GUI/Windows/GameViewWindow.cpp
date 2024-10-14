#include "GameViewWindow.h"
#include "RenderPasses.h"
#include "EventSystem.h"
#include "imgui.h"
#include "ResourceManager.h"
#include "Editor.h"
#include "Camera.h"
#include "Renderer.h"
#include "VulkanRenderTexture.h"
#include "GUIManager.h"

namespace Odyssey
{
	GameViewWindow::GameViewWindow(size_t windowID)
		: DockableWindow("Game View", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		// Rendering stuff
		m_GameViewPass = std::make_shared<OpaquePass>();
		m_GameViewPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		Renderer::PushRenderPass(m_GameViewPass);
		
		CreateRenderTexture();

		// Listen for the scene loaded event
		auto onSceneLoaded = [this](SceneLoadedEvent* event) { OnSceneLoaded(event); };
		m_SceneLoadListener = EventSystem::Listen<SceneLoadedEvent>(onSceneLoaded);
	}

	void GameViewWindow::Destroy()
	{
		if (m_SceneLoadListener)
		{
			EventSystem::RemoveListener<SceneLoadedEvent>(m_SceneLoadListener);
			m_SceneLoadListener = nullptr;
		}
		DestroyRenderTexture();
	}

	void GameViewWindow::Draw()
	{
		if (!Begin())
			return;

		// Display the RT as an Imgui image
		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID), ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_GameViewPass->SetColorRenderTexture(m_ColorRT);
		m_GameViewPass->SetDepthRenderTexture(m_DepthRT);

		End();
	}
	void GameViewWindow::OnWindowResize()
	{
		DestroyRenderTexture();
		CreateRenderTexture();

		if (m_MainCamera.IsValid())
		{
			if (Camera* camera = m_MainCamera.TryGetComponent<Camera>())
				camera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
		}
	}

	void GameViewWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void GameViewWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		for (auto entity : event->loadedScene->GetAllEntitiesWith<Camera>())
		{
			GameObject gameObject = GameObject(event->loadedScene, entity);
			Camera& camera = gameObject.GetComponent<Camera>();
			if (camera.IsMainCamera())
			{
				m_MainCamera = gameObject;
				camera.SetViewportSize(m_WindowSize.x, m_WindowSize.y);
			}
		}
	}

	void GameViewWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		m_ColorRT = ResourceManager::Allocate<VulkanRenderTexture>((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
		m_DepthRT = ResourceManager::Allocate<VulkanRenderTexture>((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, TextureFormat::D24_UNORM_S8_UINT);
		m_RTSampler = ResourceManager::Allocate<VulkanTextureSampler>();
		m_RenderTextureID = Renderer::AddImguiTexture(m_ColorRT, m_RTSampler);
	}

	void GameViewWindow::DestroyRenderTexture()
	{
		if (m_ColorRT)
			ResourceManager::Destroy(m_ColorRT);
		if (m_DepthRT)
			ResourceManager::Destroy(m_DepthRT);
		if (m_RTSampler)
			ResourceManager::Destroy(m_RTSampler);

		Renderer::DestroyImguiTexture(m_RenderTextureID);
	}
}