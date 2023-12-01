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
		m_RenderTexture = ResourceManager::AllocateTexture((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
		if (std::shared_ptr<VulkanRenderer> renderer = Application::GetRenderer())
		{
			m_RenderTextureID = renderer->GetImGui()->AddTexture(m_RenderTexture);
		}
		m_SceneViewPass->SetRenderTarget(m_RenderTexture);
	}

	void SceneViewWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Scene View Window", &open))
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
		m_WindowSize = m_WindowMax - m_WindowMin;

		// TODO: Re-allocate a texture if size has changed

		ImGui::Image(reinterpret_cast<void*>(m_RenderTextureID), ImVec2(m_WindowSize.x, m_WindowSize.y));

		RenderGizmos();

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SceneViewWindow::Destroy()
	{
		ResourceManager::DestroyTexture(m_RenderTexture);
	}

	void SceneViewWindow::RenderGizmos()
	{
		Scene* scene = SceneManager::GetActiveScene();

		if (m_SelectedObject != std::numeric_limits<uint32_t>::max())
		{
			Transform* component = ComponentManager::GetComponent<Transform>(m_SelectedObject);
			glm::mat4 view = scene->GetMainCamera()->GetInverseView();
			glm::mat4 proj = scene->GetMainCamera()->GetProjection();
			proj[1][1] *= -1.0f;
			glm::mat4 transform = component->GetWorldMatrix();

			ImGuizmo::SetRect(m_WindowPos.x, m_WindowPos.y, m_WindowSize.x, m_WindowSize.y);

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

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
				(ImGuizmo::OPERATION)op, ImGuizmo::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 pos;
				glm::vec3 rot;
				glm::vec3 scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scale));

				glm::vec3 currentRotation = component->GetEulerRotation();
				glm::vec3 diffRotation = rot - currentRotation;
				component->SetPosition(pos);
				component->AddRotation(diffRotation);
				component->SetScale(scale);
			}
		}
	}
}