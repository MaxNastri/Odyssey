#pragma once
#include "PropertyDrawer.h"
#include <GUIElement.h>
#include <GameObject.h>
#include <ManagedObject.hpp>

namespace Odyssey::Editor
{
	class PropertyDrawer;

	class UserScriptInspector : public Graphics::GUIElement
	{
	public:
		UserScriptInspector() = default;
		UserScriptInspector(Entities::GameObject go);

	public:
		virtual void Draw() override;
		void UpdateFields();

	private:
		void InitializeDrawers();

		void CreateDrawerFromProperty(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject);
		void CreateStringDrawer(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedObject userObject);

	private:
		Entities::GameObject gameObject;
		std::vector<std::unique_ptr<PropertyDrawer>> drawers;
	};
}