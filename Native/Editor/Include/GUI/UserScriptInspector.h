#pragma once
#include "PropertyDrawer.h"
#include <GUIElement.h>
#include <GameObject.h>
#include <ManagedObject.hpp>

namespace Odyssey
{
	class UserScript;
}

namespace Odyssey::Editor
{
	class PropertyDrawer;

	class UserScriptInspector : public Graphics::GUIElement
	{
	public:
		UserScriptInspector() = default;
		UserScriptInspector(GameObject go, UserScript* userScript, std::string_view className);

	public:
		virtual void Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(UserScript* userScript);

		void CreateDrawerFromProperty(GameObject gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject);
		void CreateStringDrawer(GameObject gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedObject userObject);

	private:
		GameObject gameObject;
		std::string userScriptFullName;
		std::string displayName;
		std::vector<std::unique_ptr<PropertyDrawer>> drawers;
	};
}