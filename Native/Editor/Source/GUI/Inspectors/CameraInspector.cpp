#include "CameraInspector.h"
#include "GameObject.h"
#include "Camera.h"
#include <imgui.h>

namespace Odyssey
{
	CameraInspector::CameraInspector(GameObject* gameObject)
	{
		m_GameObject = gameObject;

		if (Camera* camera = m_GameObject->GetComponent<Camera>())
		{
			m_FieldOfViewDrawer = FloatDrawer("Field of View", camera->GetFieldOfView(), [gameObject](float fov) { OnFieldOfViewChanged(gameObject, fov); });
			m_NearClipDrawer = FloatDrawer("Near Clip", camera->GetNearClip(), [gameObject](float nearClip) { OnNearClipChanged(gameObject, nearClip); });
			m_FarClipDrawer = FloatDrawer("Far Clip", camera->GetFarClip(), [gameObject](float farClip) { OnFarClipChanged(gameObject, farClip); });
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
	}

	void CameraInspector::OnFieldOfViewChanged(GameObject* gameObject, float fov)
	{
		if (Camera* camera = gameObject->GetComponent<Camera>())
		{
			camera->SetFieldOfView(fov);
		}
	}

	void CameraInspector::OnNearClipChanged(GameObject* gameObject, float nearClip)
	{
		if (Camera* camera = gameObject->GetComponent<Camera>())
		{
			camera->SetNearClip(nearClip);
		}
	}

	void CameraInspector::OnFarClipChanged(GameObject* gameObject, float farClip)
	{
		if (Camera* camera = gameObject->GetComponent<Camera>())
		{
			camera->SetFarClip(farClip);
		}
	}
}