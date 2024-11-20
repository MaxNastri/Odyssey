#pragma once
#include "Inspector.h"
#include "TransformInspector.h"
#include "CameraInspector.h"
#include "ScriptInspector.h"
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
		virtual bool Draw() override;

	public:
		void RefreshUserScripts();

	private:
		void CreateInspectors();
		void OnNameChanged(std::string_view name);

	private:
		GameObject m_Target;
		std::vector<std::unique_ptr<Inspector>> m_Inspectors;
		std::vector<ScriptInspector> userScriptInspectors;
		StringDrawer m_NameDrawer;
	};
}