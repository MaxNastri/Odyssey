#include "GameViewWindow.h"
#include "RenderPasses.h"
#include "EventSystem.h"
#include "imgui.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Camera.h"

namespace Odyssey
{
	GameViewWindow::GameViewWindow()
		: DockableWindow("Game View",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		// Rendering stuff
		m_GameViewPass = std::make_shared<OpaquePass>();
		m_GameViewPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

		if (m_MainCamera)
			m_MainCamera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
	}

	void GameViewWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		if (m_MainCamera = event->loadedScene->GetMainCamera())
			m_MainCamera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
	}

	void GameViewWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		m_ColorRT = ResourceManager::AllocateRenderTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
		m_DepthRT = ResourceManager::AllocateRenderTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, TextureFormat::D24_UNORM_S8_UINT);
		m_RTSampler = ResourceManager::AllocateSampler();

		// Create an IMGui texture handle
		if (auto renderer = Application::GetRenderer())
			if (auto imgui = renderer->GetImGui())
				m_RenderTextureID = imgui->AddTexture(m_ColorRT, m_RTSampler);
	}

	void GameViewWindow::DestroyRenderTexture()
	{
		if (m_ColorRT)
			ResourceManager::DestroyRenderTexture(m_ColorRT);
		if (m_DepthRT)
			ResourceManager::DestroyRenderTexture(m_DepthRT);
		if (m_RTSampler)
			ResourceManager::DestroySampler(m_RTSampler);

		// Create an IMGui texture handle
		// TODO: Fix this with render command queue
		//if (auto renderer = Application::GetRenderer())
		//	if (auto imgui = renderer->GetImGui())
		//		imgui->RemoveTexture(m_RenderTextureID);
	}
}