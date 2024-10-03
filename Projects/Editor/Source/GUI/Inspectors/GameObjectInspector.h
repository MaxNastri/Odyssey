#pragma once
#include "Inspector.h"
#include "TransformInspector.h"
#include "CameraInspector.h"
#include "UserScriptInspector.h"
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
		void RefreshUserScripts();

	private:
		void CreateInspectors();
		void OnNameChanged(std::string_view name);

	private:
		GameObject m_Target;
		std::vector<std::unique_ptr<Inspector>> m_Inspectors;
		std::vector<UserScriptInspector> userScriptInspectors;
		StringDrawer m_NameDrawer;
	};
}