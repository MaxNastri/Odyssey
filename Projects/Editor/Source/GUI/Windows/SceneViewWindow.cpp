#include "SceneViewWindow.h"
#include <imgui.h>
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Transform.h"
#include "ImGuizmo.h"
#include "ResourceManager.h"
#include "VulkanRenderTexture.h"
#include "VulkanRenderer.h"
#include "VulkanImgui.h"
#include "Editor.h"
#include "RenderPasses.h"
#include "Input.h"
#include "Renderer.h"
#include "EditorComponents.h"
#include "PropertiesComponent.h"

namespace Odyssey
{
	SceneViewWindow::SceneViewWindow()
		: DockableWindow("Scene View",
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		// Rendering stuff
		m_SceneViewPass = std::make_shared<OpaquePass>();
		m_SceneViewPass->SetLayouts(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		Renderer::PushRenderPass(m_SceneViewPass);

		// Create the render texture
		CreateRenderTexture();

		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });
	}

	void SceneViewWindow::Destroy()
	{
		if (m_SceneLoadedListener)
		{
			EventSystem::RemoveListener<SceneLoadedEvent>(m_SceneLoadedListener);
			m_SceneLoadedListener = nullptr;
		}
		DestroyRenderTexture();
	}

	void SceneViewWindow::Update()
	{
		// Reset the camera controller use flag before updating the camera controller
		m_CameraControllerInUse = false;

		if (m_GameObject)
			UpdateCameraController();

		if (!m_CameraControllerInUse)
		{
			UpdateGizmosInput();
		}
	}

	void SceneViewWindow::Draw()
	{
		if (!Begin())
			return;

		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID), ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_SceneViewPass->SetColorRenderTexture(m_ColorRT);
		m_SceneViewPass->SetDepthRenderTexture(m_DepthRT);

		// Render gizmos
		RenderGizmos();
		End();
	}

	void SceneViewWindow::OnWindowResize()
	{
		DestroyRenderTexture();
		CreateRenderTexture();

		if (m_GameObject)
		{
			if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
				camera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
		}
	}

	void SceneViewWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		// Create a new game object and mark it as hidden
		if (Scene* activeScene = event->loadedScene)
		{
			m_GameObject = activeScene->CreateGameObject();
			m_GameObject.AddComponent<Transform>();

			// Add a transform and camera
			Camera& camera = m_GameObject.AddComponent<Camera>();
			camera.SetMainCamera(false);
			camera.Awake();
			camera.SetViewportSize(m_WindowSize.x, m_WindowSize.y);

			// Add the editor properties component to hide this object in the scene hierarchy window
			EditorPropertiesComponent& properties = m_GameObject.AddComponent<EditorPropertiesComponent>();
			properties.ShowInHierarchy = false;

			// Make sure we don't serialize this game object
			PropertiesComponent& engineProperties = m_GameObject.GetComponent<PropertiesComponent>();
			engineProperties.Serialize = false;

			m_SceneViewPass->SetCamera(&camera);
		}
	}

	void SceneViewWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		m_ColorRT = ResourceManager::AllocateRenderTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
		m_DepthRT = ResourceManager::AllocateRenderTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y, TextureFormat::D24_UNORM_S8_UINT);
		m_RTSampler = ResourceManager::AllocateSampler();
		m_RenderTextureID = Renderer::AddImguiTexture(m_ColorRT, m_RTSampler);
	}

	void SceneViewWindow::DestroyRenderTexture()
	{
		// Destroy the existing render texture
		if (m_ColorRT.IsValid())
		{
			// Destroy the render texture
			ResourceManager::DestroyRenderTexture(m_ColorRT);
			ResourceManager::DestroyRenderTexture(m_DepthRT);
			ResourceManager::DestroySampler(m_RTSampler);

			// Create an IMGui texture handle
		// TODO: Fix this with render command queue
			//if (auto renderer = Application::GetRenderer())
			//	if (auto imgui = renderer->GetImGui())
			//		 imgui->RemoveTexture(m_RenderTextureID);
		}
	}

	void SceneViewWindow::RenderGizmos()
	{/*
		if (m_SelectedObject)
		{
			if (Transform* transform = m_SelectedObject->GetComponent<Transform>())
			{
				ImGuizmo::SetRect(m_WindowPos.x, m_WindowPos.y, m_WindowSize.x, m_WindowSize.y);

				glm::mat4 worldMatrix = transform->GetWorldMatrix();
				glm::mat4 view = m_Camera->GetInverseView();
				glm::mat4 proj = m_Camera->GetProjection();
				proj[1][1] *= -1.0f;

				ImGuizmo::AllowAxisFlip(false);
				ImGuizmo::SetGizmoSizeClipSpace(0.1f);
				ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
					(ImGuizmo::OPERATION)op, ImGuizmo::WORLD, glm::value_ptr(worldMatrix));

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 pos, rot, scale;
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(worldMatrix), glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scale));

					glm::vec3 currentRotation = transform->GetEulerRotation();
					glm::vec3 diffRotation = rot - currentRotation;

					if (op == ImGuizmo::OPERATION::TRANSLATE)
						transform->SetPosition(pos);
					else if (op == ImGuizmo::ROTATE)
						transform->AddRotation(diffRotation);
					else if (op == ImGuizmo::SCALE)
						transform->SetScale(scale);
				}
			}
		}*/
	}

	void SceneViewWindow::UpdateCameraController()
	{
		const float speed = 3.0f;

		glm::vec3 inputVel = glm::zero<vec3>();
		Transform* transform = m_GameObject.TryGetComponent<Transform>();

		if (!transform)
			return;

		if (m_CursorInContentRegion && Input::GetMouseButtonDown(MouseButton::Right))
		{
			m_CameraControllerInUse = true;

			if (Input::GetKeyDown(KeyCode::W))
			{
				inputVel += glm::vec3(0, 0, 1);
			}
			if (Input::GetKeyDown(KeyCode::S))
			{
				inputVel += glm::vec3(0, 0, -1);
			}
			if (Input::GetKeyDown(KeyCode::D))
			{
				inputVel += glm::vec3(1, 0, 0);
			}
			if (Input::GetKeyDown(KeyCode::A))
			{
				inputVel += glm::vec3(-1, 0, 0);
			}
			if (Input::GetKeyDown(KeyCode::E))
			{
				inputVel += glm::vec3(0, 1, 0);
			}
			if (Input::GetKeyDown(KeyCode::Q))
			{
				inputVel += glm::vec3(0, -1, 0);
			}

			if (inputVel != glm::zero<vec3>())
			{
				inputVel = glm::normalize(inputVel);

				glm::vec3 right = transform->Right() * inputVel.x;
				glm::vec3 up = transform->Up() * inputVel.y;
				glm::vec3 fwd = transform->Forward() * inputVel.z;
				glm::vec3 velocity = (right + up + fwd) * speed * (1.0f / 144.0f);
				transform->AddPosition(velocity);
			}

			float mouseH = (float)Input::GetMouseAxisHorizontal();
			float mouseV = (float)Input::GetMouseAxisVerticle();

			if (mouseH != 0.0f || mouseV != 0.0f)
			{
				glm::vec3 yaw = vec3(0, 1, 0) * mouseH * (1.0f / 144.0f) * 15.0f;
				glm::vec3 pitch = vec3(1, 0, 0) * mouseV * (1.0f / 144.0f) * 15.0f;

				transform->AddRotation(yaw);
				transform->AddRotation(pitch);
			}
		}
	}

	void SceneViewWindow::UpdateGizmosInput()
	{
		if (m_CursorInContentRegion)
		{
			if (Input::GetKeyPress(KeyCode::Q))
			{
				// Translation
				op = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (Input::GetKeyPress(KeyCode::W))
			{
				// ROTATION
				op = ImGuizmo::OPERATION::ROTATE;
			}
			else if (Input::GetKeyPress(KeyCode::E))
			{
				// SCALE
				op = ImGuizmo::OPERATION::SCALE;
			}
		}
	}
}