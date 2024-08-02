#pragma once
#include "Inspector.h"
#include "PropertyDrawer.h"
#include "GameObject.h"
#include "ManagedObject.hpp"

namespace Odyssey
{
	class PropertyDrawer;
	class ScriptComponent;

	class UserScriptInspector : public Inspector
	{
	public:
		UserScriptInspector() = default;
		UserScriptInspector(GameObject& go);

	public:
		virtual void Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(ScriptComponent* userScript);

		void CreateDrawerFromProperty(GameObject& gameObject, const std::string& fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject);
		void CreateStringDrawer(GameObject& gameObject, const std::string& fieldName, Coral::ManagedObject userObject);

	private:
		GameObject m_GameObject;
		std::string userScriptFullName;
		std::string displayName;
		std::vector<std::shared_ptr<PropertyDrawer>> drawers;
	};
}