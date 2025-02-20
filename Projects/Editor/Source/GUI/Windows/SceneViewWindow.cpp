#include "SceneViewWindow.h"
#include <imgui.h>
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Transform.h"
#include "ImGuizmo.h"
#include "ResourceManager.h"
#include "VulkanRenderer.h"
#include "VulkanImgui.h"
#include "Editor.h"
#include "RenderPasses.h"
#include "Input.h"
#include "Renderer.h"
#include "EditorComponents.h"
#include "PropertiesComponent.h"
#include "DebugRenderer.h"
#include "GUIManager.h"
#include "Preferences.h"
#include "RenderTarget.h"
#include "VulkanTextureSampler.h"
#include "OdysseyTime.h"
#include "ThumbnailManager.h"
#include "Gizmos.h"

namespace Odyssey
{
	SceneViewWindow::SceneViewWindow(size_t windowID)
		: DockableWindow("Scene View", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
		// Rendering stuff
		m_SceneViewPass = new RenderObjectsPass();
		m_SceneViewPass->AddDebugSubPass();
		m_SceneViewPass->SetCamera((uint8_t)Camera::Tag::SceneView);

		m_TransparentPass = new TransparentObjectsPass();
		m_TransparentPass->SetCamera((uint8_t)Camera::Tag::SceneView);
		Renderer::PushRenderPass(m_SceneViewPass);
		Renderer::PushRenderPass(m_TransparentPass);

		// Create the render texture
		CreateRenderTexture();
		Gizmos::Init();
		m_GizmoIcon = ThumbnailManager::LoadThumbnail(Preferences::GetGizmoIcon());

		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* eventData) { OnSceneLoaded(eventData); });

		m_GUISelectionListener = EventSystem::Listen<GUISelectionChangedEvent>
			([this](GUISelectionChangedEvent* eventData) { OnGUISelectionChanged(eventData); });

		m_PlaymodeStateChangedListener = EventSystem::Listen<PlaymodeStateChangedEvent>
			([this](PlaymodeStateChangedEvent* eventData) { OnPlaymodeStateChanged(eventData); });
	}

	void SceneViewWindow::Destroy()
	{
		if (m_SceneLoadedListener)
		{
			EventSystem::RemoveListener<SceneLoadedEvent>(m_SceneLoadedListener);
			m_SceneLoadedListener = nullptr;
		}
		if (m_GUISelectionListener)
		{
			EventSystem::RemoveListener<GUISelectionChangedEvent>(m_GUISelectionListener);
			m_GUISelectionListener = nullptr;
		}
		if (m_PlaymodeStateChangedListener)
		{
			EventSystem::RemoveListener<PlaymodeStateChangedEvent>(m_PlaymodeStateChangedListener);
			m_PlaymodeStateChangedListener = nullptr;
		}
		DestroyRenderTexture();
	}

	void SceneViewWindow::Update()
	{
		// Reset the camera controller use flag before updating the camera controller
		m_CameraControllerInUse = false;

		if (m_GameObject.IsValid() && m_AllowInput)
			UpdateCameraController();

		if (!m_CameraControllerInUse)
		{
			UpdateGizmosInput();
		}
	}

	bool SceneViewWindow::Draw()
	{
		bool modified = false;

		if (!Begin())
			return modified;

		float2 startCursor = ImGui::GetCursorPos();

		// Render the scene view
		ImGui::Image((void*)m_RenderTextureID, ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_SceneViewPass->SetRenderTarget(m_RenderTarget);
		m_TransparentPass->SetRenderTarget(m_RenderTarget);

		// Draw the menu bar
		DrawMenuBar(startCursor);

		// Render gizmos
		RenderGizmos();

		End();
		return modified;
	}

	void SceneViewWindow::OnWindowResize()
	{
		DestroyRenderTexture();
		CreateRenderTexture();

		if (m_GameObject.IsValid())
		{
			if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
				camera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
		}
	}

	void SceneViewWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void SceneViewWindow::OnSceneLoaded(SceneLoadedEvent* eventData)
	{
		// Create a new game object and mark it as hidden
		if (m_ActiveScene = eventData->loadedScene)
		{
			m_GameObject = m_ActiveScene->CreateGameObject();
			Transform& transform = m_GameObject.AddComponent<Transform>();

			// Add a transform and camera
			Camera& camera = m_GameObject.AddComponent<Camera>();
			camera.SetMainCamera(false);
			camera.Awake();
			camera.SetViewportSize(m_WindowSize.x, m_WindowSize.y);
			camera.SetTag(Camera::Tag::SceneView);
			transform.SetLocalMatrix(Preferences::GetSceneView());

			// Add the editor properties component to hide this object in the scene hierarchy window
			EditorPropertiesComponent& properties = m_GameObject.AddComponent<EditorPropertiesComponent>();
			properties.ShowInHierarchy = false;

			// Make sure we don't serialize this game object
			PropertiesComponent& engineProperties = m_GameObject.GetComponent<PropertiesComponent>();
			engineProperties.Serialize = false;
		}
	}

	void SceneViewWindow::OnGUISelectionChanged(GUISelectionChangedEvent* eventData)
	{
		if (eventData->Selection.Type == GameObject::Type)
			m_SelectedGO = m_ActiveScene->GetGameObject(eventData->Selection.GUID);
	}

	void SceneViewWindow::OnPlaymodeStateChanged(PlaymodeStateChangedEvent* eventData)
	{
		switch (eventData->State)
		{
			case PlaymodeState::EnterPlaymode:
				m_AllowInput = false;
				break;
			case PlaymodeState::ExitPlaymode:
				m_AllowInput = true;
				break;
		}
	}

	void SceneViewWindow::CreateRenderTexture()
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		VulkanImageDescription desc;
		desc.ImageType = ImageType::RenderTexture;
		desc.Width = (uint32_t)m_WindowSize.x;
		desc.Height = (uint32_t)m_WindowSize.y;
		desc.Samples = 8;

		m_RenderTarget = ResourceManager::Allocate<RenderTarget>(desc, RenderTargetFlags::Color | RenderTargetFlags::Depth);
		m_RTSampler = ResourceManager::Allocate<VulkanTextureSampler>();
		m_RenderTextureID = Renderer::AddImguiRenderTexture(m_RenderTarget, m_RTSampler);
	}

	void SceneViewWindow::DestroyRenderTexture()
	{
		// Destroy the existing render texture
		if (m_RenderTarget.IsValid())
		{
			// Destroy the render texture
			ResourceManager::Destroy(m_RenderTarget);
			ResourceManager::Destroy(m_RTSampler);
			Renderer::DestroyImguiTexture(m_RenderTextureID);
		}
	}

	void SceneViewWindow::DrawMenuBar(float2 menuPosition)
	{
		float2 buttonSize = float2(ImGui::GetFrameHeight());
		float4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
		const float yPadding = 2.0f;
		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x * 0.0f;
		const float yAnchor = menuPosition.y - m_WindowPadding.y + yPadding;

		// Background
		ImGui::FilledRectSpan(bgColor, 30.0f, float2(0.0f));

		// Gizmo button
		float2 position = float2(ImGui::GetContentRegionAvail().x - buttonSize.x + m_WindowPadding.x, yAnchor);
		ImGui::SetCursorPos(position);

		if (ImGui::ImageButton((void*)m_GizmoIcon, buttonSize, float2(0.0f), float2(1.0f), 0))
		{
			if (ImGui::IsPopupOpen("##GizmoPopup"))
				ImGui::CloseCurrentPopup();
			else
				ImGui::OpenPopup("##GizmoPopup");
		}

		if (ImGui::BeginPopup("##GizmoPopup"))
		{
			for (auto& [componentName, info] : Gizmos::GetGizmoInfo())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

				if (ImGui::MenuItem(componentName.c_str(), "", &info.Enabled))
					info.OnGizmoModified(info.Enabled);

				ImGui::PopItemFlag();
			}
			ImGui::EndPopup();
		}
	}

	void SceneViewWindow::RenderGizmos()
	{
		if (m_SelectedGO.IsValid())
		{
			if (Transform* transform = m_SelectedGO.TryGetComponent<Transform>())
			{
				Camera& camera = m_GameObject.GetComponent<Camera>();
				ImGuizmo::SetRect(m_WindowPos.x, m_WindowPos.y, m_WindowSize.x, m_WindowSize.y);

				glm::mat4 worldMatrix = transform->GetWorldMatrix();
				glm::mat4 view = camera.GetInverseView();
				glm::mat4 proj = camera.GetProjection();

				ImGuizmo::AllowAxisFlip(SceneViewWindow::AllowFlip);
				ImGuizmo::SetGizmoSizeClipSpace(0.1f);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(m_WindowPos.x, m_WindowPos.y, m_WindowSize.x, m_WindowSize.y);
				ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
					(ImGuizmo::OPERATION)op, SceneViewWindow::IsLocal ? ImGuizmo::LOCAL : ImGuizmo::WORLD, glm::value_ptr(worldMatrix));

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation;
					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(worldMatrix, scale, rotation, translation, skew, perspective);

					if (op == ImGuizmo::OPERATION::TRANSLATE)
					{
						transform->SetPosition(translation);
						transform->SetLocalSpace();
					}
					else if (op == ImGuizmo::ROTATE)
					{
						transform->SetRotation(rotation);
						transform->SetLocalSpace();
					}
					else if (op == ImGuizmo::SCALE)
					{
						transform->SetScale(scale);
						transform->SetLocalSpace();
					}
				}
			}
		}
	}

	void SceneViewWindow::UpdateCameraController()
	{
		const float speed = 20.0f;

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
				glm::vec3 velocity = (right + up + fwd) * speed * Time::DeltaTime();
				transform->SetPosition(transform->GetPosition() + velocity);
			}

			float mouseH = (float)Input::GetMouseAxisHorizontal();
			float mouseV = (float)Input::GetMouseAxisVertical();

			if (mouseH != 0.0f || mouseV != 0.0f)
			{
				float yaw = mouseH * Time::DeltaTime() * 15.0f;
				float pitch = mouseV * Time::DeltaTime() * 15.0f;

				float3 euler = transform->GetEulerRotation(false);
				transform->SetRotation(float3(euler.x + pitch, euler.y + yaw, 0.0f), false);
			}

			// TODO: Move this to a timer or (ideally) into one of the destroy functions
			if (inputVel != float3(0.0f) || mouseH != 0.0f || mouseV != 0.0f)
				Preferences::SetSceneView(transform->GetWorldMatrix());
		}
	}

	void SceneViewWindow::UpdateGizmosInput()
	{
		if (m_CursorInContentRegion && m_AllowInput)
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