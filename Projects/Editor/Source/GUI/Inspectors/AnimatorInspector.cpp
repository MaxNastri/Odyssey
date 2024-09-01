#include "AnimatorInspector.h"
#include "Animator.h"
#include "imgui.h"

namespace Odyssey
{
	AnimatorInspector::AnimatorInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
		{
			GUID rigGUID = animator->GetRig();
			GUID clipGUID = animator->GetClip();

			m_RigDrawer = AssetFieldDrawer("Rig", rigGUID, "AnimationRig",
				[this](GUID guid) { OnRigModified(guid); });

			m_ClipDrawer = AssetFieldDrawer("Clip", clipGUID, "AnimationClip",
				[this](GUID guid) { OnClipModified(guid); });
		}
	}

	void AnimatorInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_RigDrawer.Draw();
			m_ClipDrawer.Draw();

			if (ImGui::Button("Play"))
			{
				if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
					animator->Play();
			}
			else if (ImGui::Button("Pause"))
			{
				if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
					animator->Pause();
			}
		}
	}

	void AnimatorInspector::OnRigModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetRig(guid);
	}

	void AnimatorInspector::OnClipModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetClip(guid);
	}
}