#include "AnimatorInspector.h"
#include "Animator.h"
#include "AnimationClip.h"
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

			m_RigDrawer = AssetFieldDrawer("Rig", rigGUID, AnimationRig::Type,
				[this](GUID guid) { OnRigModified(guid); });

			m_ClipDrawer = AssetFieldDrawer("Clip", clipGUID, AnimationClip::Type,
				[this](GUID guid) { OnClipModified(guid); });

			m_DebugEnabledDrawer = BoolDrawer("Debug", false,
				[this](bool enabled) { OnDebugEnabledModified(enabled); });
		}
	}

	void AnimatorInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_RigDrawer.Draw();
			m_ClipDrawer.Draw();
			m_DebugEnabledDrawer.Draw();

			if (ImGui::Button("Play"))
			{
				if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
					animator->Play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
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
	void AnimatorInspector::OnDebugEnabledModified(bool enabled)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetDebugEnabled(enabled);
	}
}