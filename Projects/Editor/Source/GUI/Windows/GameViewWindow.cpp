#include "Camera.h"
#include "Editor.h"
#include "EventSystem.h"
#include "GameViewWindow.h"
#include "GUIManager.h"
#include "imgui.h"
#include "Renderer.h"
#include "RenderPasses.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "RenderTarget.h"
#include "VulkanTextureSampler.h"
#include "Input.h"

namespace Odyssey
{
	GameViewWindow::GameViewWindow(size_t windowID)
		: DockableWindow("Game View", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		// Rendering stuff
		m_GameViewPass = new OpaquePass();
		m_GameViewPass->SetCamera((uint8_t)Camera::Tag::Main);
		m_GameViewPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		m_GameViewPass->Add2DSubPass();
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

	void GameViewWindow::Update()
	{
		if (Input::GetKeyDown(KeyCode::Escape))
		{
			Renderer::ReleaseCursor();
		}
	}

	bool GameViewWindow::Draw()
	{
		bool modified = false;

		if (!Begin())
			return modified;

		if (m_CursorInContentRegion && ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy) && Input::GetMouseButtonDown(MouseButton::Left))
			Renderer::CaptureCursor();

		// Display the RT as an Imgui image
		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID), ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_GameViewPass->SetRenderTarget(m_RenderTarget);

		End();
		return modified;
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
				camera.SetTag(Camera::Tag::Main);
			}
		}
	}

	void GameViewWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass

		VulkanImageDescription desc;
		desc.ImageType = ImageType::RenderTexture;
		desc.Width = (uint32_t)m_WindowSize.x;
		desc.Height = (uint32_t)m_WindowSize.y;

		m_RenderTarget = ResourceManager::Allocate<RenderTarget>(desc, RenderTargetFlags::Color | RenderTargetFlags::Depth);
		m_RTSampler = ResourceManager::Allocate<VulkanTextureSampler>();
		m_RenderTextureID = Renderer::AddImguiRenderTexture(m_RenderTarget, m_RTSampler);
	}

	void GameViewWindow::DestroyRenderTexture()
	{
		if (m_RenderTarget)
			ResourceManager::Destroy(m_RenderTarget);
		if (m_RTSampler)
			ResourceManager::Destroy(m_RTSampler);

		Renderer::DestroyImguiTexture(m_RenderTextureID);
	}
}