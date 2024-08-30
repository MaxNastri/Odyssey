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

			m_RigDrawer = AssetFieldDrawer("Rig", rigGUID, "AnimationRig",
				[this](GUID guid) { OnRigModified(guid); });
		}
	}

	void AnimatorInspector::Draw()
	{
		if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_RigDrawer.Draw();
		}
	}

	void AnimatorInspector::OnRigModified(GUID guid)
	{
		if (Animator* animator = m_GameObject.TryGetComponent<Animator>())
		{
			animator->SetRig(guid);
		}
	}
}