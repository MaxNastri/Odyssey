#pragma once
#include "PropertyDrawer.h"
#include <GUIElement.h>
#include <GameObject.h>
#include <ManagedObject.hpp>

namespace Odyssey::Entities
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
		UserScriptInspector(Entities::GameObject go, Entities::UserScript* userScript, std::string_view className);

	public:
		virtual void Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers(Entities::UserScript* userScript);

		void CreateDrawerFromProperty(Entities::GameObject gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject);
		void CreateStringDrawer(Entities::GameObject gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedObject userObject);

	private:
		Entities::GameObject gameObject;
		std::string userScriptFullName;
		std::string displayName;
		std::vector<std::unique_ptr<PropertyDrawer>> drawers;
	};
}