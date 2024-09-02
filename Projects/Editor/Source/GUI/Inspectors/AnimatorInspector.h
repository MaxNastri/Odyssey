#pragma once
#include "Inspector.h"
#include "GameObject.h"
#include "AssetFieldDrawer.h"
#include "BoolDrawer.h"

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
		void OnClipModified(GUID guid);
		void OnDebugEnabledModified(bool enabled);

	private:
		GameObject m_GameObject;
		AssetFieldDrawer m_RigDrawer;
		AssetFieldDrawer m_ClipDrawer;
		BoolDrawer m_DebugEnabledDrawer;
	};
}