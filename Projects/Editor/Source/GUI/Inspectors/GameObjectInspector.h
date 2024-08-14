#pragma once
#include "Inspector.h"
#include "TransformInspector.h"
#include "CameraInspector.h"
#include "UserScriptInspector.h"
#include "StringDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class GameObject;

	class GameObjectInspector : public Inspector
	{
	public:
		GameObjectInspector() = default;
		GameObjectInspector(GUID guid);

	public:
		virtual void Draw() override;

	public:
		void SetGameObject(GameObject& gameObject);
		void RefreshUserScripts();

	private:
		std::vector<std::unique_ptr<Inspector>> m_Inspectors;
		std::vector<UserScriptInspector> userScriptInspectors;
		StringDrawer m_NameDrawer;
	};
}