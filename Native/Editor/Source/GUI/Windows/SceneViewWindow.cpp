#include "SceneViewWindow.h"
#include <imgui.h>
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"
#include "ComponentManager.h"
#include "ImGuizmo.h"
#include "ResourceManager.h"
#include "VulkanTexture.h"
#include "VulkanRenderer.h"
#include "VulkanImgui.h"
#include "Application.h"
#include "RenderPasses.h"
#include "Input.h"

namespace Odyssey
{
	SceneViewWindow::SceneViewWindow()
	{
		// Rendering stuff
		m_SceneViewPass = std::make_shared<OpaquePass>();
		m_SceneViewPass->SetLayouts(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// Window stuff
		m_WindowPos = glm::vec2(0, 0);
		m_WindowSize = glm::vec2(500, 500);

		m_RenderTexture.resize(2);
		m_RenderTextureID.resize(2);

		// Create the render texture
		CreateRenderTexture(0);
		CreateRenderTexture(1);

		m_GameObject.id = std::numeric_limits<uint32_t>::max();

		m_CameraTransform = ComponentManager::AddComponent<Transform>(m_GameObject.id);
		m_CameraTransform->SetGameObject(&m_GameObject);
		m_CameraTransform->Awake();

		m_Camera = ComponentManager::AddComponent<Camera>(m_GameObject.id);
		m_Camera->SetGameObject(&m_GameObject);
		m_Camera->Awake();

		m_SceneViewPass->SetCamera(m_Camera);
	}

	void SceneViewWindow::Update()
	{
		// Reset the camera controller use flag before updating the camera controller
		m_CameraControllerInUse = false;
		UpdateCameraController();

		if (!m_CameraControllerInUse)
		{
			UpdateGizmosInput();
		}
	}

	void SceneViewWindow::Draw()
	{
		uint32_t frameIndex = VulkanRenderer::GetFrameIndex();

		ImGui::SetNextWindowSize(ImVec2(m_WindowSize.x, m_WindowSize.y), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Scene View", &open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		// Draw
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 min = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();

		m_WindowPos = glm::vec2(pos.x, pos.y);
		m_WindowMin = glm::vec2(min.x, min.y) + m_WindowPos;
		m_WindowMax = glm::vec2(max.x, max.y) + m_WindowPos;
		glm::vec2 windowSize = m_WindowMax - m_WindowMin;

		// TODO: Re-allocate a texture if size has changed
		if (windowSize != m_WindowSize)
		{
			m_WindowSize = windowSize;
			DestroyRenderTexture(frameIndex);
			CreateRenderTexture(frameIndex);
			m_Camera->SetViewportSize(m_WindowSize.x, m_WindowSize.y);
		}

		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID[frameIndex]), ImVec2(m_WindowSize.x, m_WindowSize.y));
		m_SceneViewPass->SetRenderTexture(m_RenderTexture[frameIndex]);

		RenderGizmos();

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SceneViewWindow::Destroy()
	{
		DestroyRenderTexture(0);
		DestroyRenderTexture(1);
	}

	void SceneViewWindow::CreateRenderTexture(uint32_t index)
	{
		// Create a new render texture at the correct size and set it as the render target for the scene view pass
		m_RenderTexture[index] = ResourceManager::AllocateTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);

		// Create an IMGui texture handle
		if (auto renderer = Application::GetRenderer())
			if (auto imgui = renderer->GetImGui())
				m_RenderTextureID[index] = imgui->AddTexture(m_RenderTexture[index]);
	}

	void SceneViewWindow::DestroyRenderTexture(uint32_t index)
	{
		// Destroy the existing render texture
		if (m_RenderTexture[index].IsValid())
		{
			// Remove the imgui texture
			if (auto renderer = Application::GetRenderer())
				if (auto imgui = renderer->GetImGui())
					//imgui->RemoveTexture(m_RenderTextureID[index]);

			// Destroy the render texture
					ResourceManager::DestroyTexture(m_RenderTexture[index], index);
		}
	}

	void SceneViewWindow::RenderGizmos()
	{
		if (m_SelectedObject != std::numeric_limits<uint32_t>::max())
		{
			ImGuizmo::SetRect(m_WindowPos.x, m_WindowPos.y, m_WindowSize.x, m_WindowSize.y);

			glm::mat4 transform = m_CameraTransform->GetWorldMatrix();
			glm::mat4 view = m_Camera->GetInverseView();
			glm::mat4 proj = m_Camera->GetProjection();
			proj[1][1] *= -1.0f;

			ImGuizmo::AllowAxisFlip(false);
			ImGuizmo::SetGizmoSizeClipSpace(0.1f);
			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
				(ImGuizmo::OPERATION)op, ImGuizmo::WORLD, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 pos, rot, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scale));

				glm::vec3 currentRotation = m_CameraTransform->GetEulerRotation();
				glm::vec3 diffRotation = rot - currentRotation;

				if (op == ImGuizmo::OPERATION::TRANSLATE)
					m_CameraTransform->SetPosition(pos);
				else if (op == ImGuizmo::ROTATE)
					m_CameraTransform->AddRotation(diffRotation);
				else if (op == ImGuizmo::SCALE)
					m_CameraTransform->SetScale(scale);
			}
		}
	}

	void SceneViewWindow::UpdateCameraController()
	{
		const float speed = 3.0f;

		glm::vec3 inputVel = glm::zero<vec3>();

		if (Input::GetMouseButtonDown(MouseButton::Right))
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
				glm::vec3 right = m_CameraTransform->Right() * inputVel.x;
				glm::vec3 up = m_CameraTransform->Up() * inputVel.y;
				glm::vec3 fwd = m_CameraTransform->Forward() * inputVel.z;
				glm::vec3 velocity = (right + up + fwd) * speed * (1.0f / 144.0f);
				m_CameraTransform->AddPosition(velocity);
			}

			float mouseH = (float)Input::GetMouseAxisHorizontal();
			float mouseV = (float)Input::GetMouseAxisVerticle();

			if (mouseH != 0.0f || mouseV != 0.0f)
			{
				glm::vec3 yaw = vec3(0, 1, 0) * mouseH * (1.0f / 144.0f) * 15.0f;
				glm::vec3 pitch = vec3(1, 0, 0) * mouseV * (1.0f / 144.0f) * 15.0f;

				m_CameraTransform->AddRotation(yaw);
				m_CameraTransform->AddRotation(pitch);
			}
		}
	}

	void SceneViewWindow::UpdateGizmosInput()
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