#pragma once
#include "Animator.h"
#include "Inspector.h"
#include "GameObject.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class AnimatorInspector : public Inspector
	{
	public:
		AnimatorInspector() = default;
		AnimatorInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnRigModified(GUID guid);
		void OnBlueprintModified(GUID guid);
		void OnDebugEnabledModified(bool enabled);

	private:
		bool m_AnimatorEnabled;
		GameObject m_GameObject;
		AssetFieldDrawer m_RigDrawer;
		AssetFieldDrawer m_BlueprintDrawer;
		BoolDrawer m_DebugEnabledDrawer;
	};
}