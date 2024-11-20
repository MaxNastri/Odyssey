#include "CameraInspector.h"
#include "GameObject.h"
#include "Camera.h"
#include <imgui.h>

namespace Odyssey
{
	CameraInspector::CameraInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			m_CameraEnabled = camera->IsEnabled();
			m_FieldOfViewDrawer = FloatDrawer("Field of View", camera->GetFieldOfView(),
				[this](float fov) { OnFieldOfViewChanged(fov); });
			
			m_NearClipDrawer = FloatDrawer("Near Clip", camera->GetNearClip(),
				[this](float nearClip) { OnNearClipChanged(nearClip); });
			
			m_FarClipDrawer = FloatDrawer("Far Clip", camera->GetFarClip(),
				[this](float farClip) { OnFarClipChanged(farClip); });
		}
	}

	bool CameraInspector::Draw()
	{
		bool modified = false;

		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_CameraEnabled))
		{
			if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
				camera->SetEnabled(m_CameraEnabled);
			modified = true;
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			modified |= m_FieldOfViewDrawer.Draw();
			modified |= m_NearClipDrawer.Draw();
			modified |= m_FarClipDrawer.Draw();
		}

		ImGui::PopID();

		return modified;
	}

	void CameraInspector::OnFieldOfViewChanged(float fov)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetFieldOfView(fov);
		}
	}

	void CameraInspector::OnNearClipChanged(float nearClip)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetNearClip(nearClip);
		}
	}

	void CameraInspector::OnFarClipChanged(float farClip)
	{
		if (Camera* camera = m_GameObject.TryGetComponent<Camera>())
		{
			camera->SetFarClip(farClip);
		}
	}
}