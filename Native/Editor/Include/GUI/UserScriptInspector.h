#pragma once
#include <GUIElement.h>
#include <GameObject.h>
#include <string>
#include <unordered_map>
#include <PropertyDrawer.h>
#include <Utility.hpp>
#include <ManagedObject.hpp>

namespace Odyssey::Editor
{
	class UserScriptInspector : public Graphics::GUIElement
	{
	public:
		UserScriptInspector() = default;
		UserScriptInspector(Entities::GameObject go);

	public:
		virtual void Draw() override;

	private:
		void CreateDrawerFromProperty(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject);

	private:
		Entities::GameObject gameObject;
		std::vector<std::unique_ptr<PropertyDrawer>> drawers;
	};
}