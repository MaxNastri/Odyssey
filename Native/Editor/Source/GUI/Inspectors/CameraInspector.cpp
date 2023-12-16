#include "CameraInspector.h"
#include <Camera.h>
#include <ComponentManager.h>
#include <imgui.h>

namespace Odyssey
{
	CameraInspector::CameraInspector(GameObject* gameObject)
	{
		m_GameObject = gameObject;

		if (Camera* camera = m_GameObject->GetComponent<Camera>())
		{
			std::function<void(float)> fovModified = [gameObject](float fov)
				{
					if (Camera* camera = gameObject->GetComponent<Camera>())
					{
						camera->SetFieldOfView(fov);
					}
				};

			std::function<void(float)> nearClipModified = [gameObject](float nearClip)
				{
					if (Camera* camera = gameObject->GetComponent<Camera>())
					{
						camera->SetNearClip(nearClip);
					}
				};

			std::function<void(float)> farClipModified = [gameObject](float farClip)
				{
					if (Camera* camera = gameObject->GetComponent<Camera>())
					{
						camera->SetFarClip(farClip);
					}
				};

			m_FieldOfViewDrawer = FloatDrawer("Field of View", camera->GetFieldOfView(), fovModified);
			m_NearClipDrawer = FloatDrawer("Near Clip", camera->GetNearClip(), nearClipModified);
			m_FarClipDrawer = FloatDrawer("Far Clip", camera->GetFarClip(), farClipModified);
		}
	}

	void CameraInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("CameraInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##A", 0, 0.4f);
				m_FieldOfViewDrawer.Draw();
				ImGui::TableNextRow();
				m_NearClipDrawer.Draw();
				ImGui::TableNextRow();
				m_FarClipDrawer.Draw();
				ImGui::EndTable();
			}
		}

		ImGui::Separator();
	}
}