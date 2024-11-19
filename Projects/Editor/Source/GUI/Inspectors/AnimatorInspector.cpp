#include "AnimatorInspector.h"
#include "AnimationBlueprint.h"
#include "imgui.h"

namespace Odyssey
{
	AnimatorInspector::AnimatorInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
		{
			m_AnimatorEnabled = animator->IsEnabled();

			m_RigDrawer = AssetFieldDrawer("Rig", animator->GetRigAsset(), AnimationRig::Type,
				[this](GUID guid) { OnRigModified(guid); });

			m_BlueprintDrawer = AssetFieldDrawer("Blueprint", animator->GetBlueprintAsset(), AnimationBlueprint::Type,
				[this](GUID guid) { OnBlueprintModified(guid); });

			m_DebugEnabledDrawer = BoolDrawer("Debug", false,
				[this](bool enabled) { OnDebugEnabledModified(enabled); });
		}
	}

	void AnimatorInspector::Draw()
	{
		ImGui::PushID(this);

		if (ImGui::Checkbox("##enabled", &m_AnimatorEnabled))
		{
			if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
				animator->SetEnabled(m_AnimatorEnabled);
		}

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_RigDrawer.Draw();
			m_BlueprintDrawer.Draw();
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

		ImGui::PopID();
	}

	void AnimatorInspector::OnRigModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetRig(guid);
	}

	void AnimatorInspector::OnBlueprintModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetBlueprint(guid);
	}
	void AnimatorInspector::OnDebugEnabledModified(bool enabled)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
			animator->SetFloat("Speed", enabled ? 10.0f : 0.0f);
	}
}