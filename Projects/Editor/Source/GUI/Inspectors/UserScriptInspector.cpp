#include "UserScriptInspector.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "StringDrawer.h"
#include "ScriptComponent.h"
#include "imgui.h"
#include "DrawerUtils.h"
#include "ScriptingManager.h"
#include "EntityFieldDrawer.h"

namespace Odyssey
{
	UserScriptInspector::UserScriptInspector(GameObject& gameObject)
	{
		m_GameObject = gameObject;

		if (ScriptComponent* scriptComponent = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			userScriptFullName = scriptComponent->GetManagedTypeName();
			displayName = userScriptFullName;

			// Remove any namespaces
			size_t found = userScriptFullName.find_last_of('.');
			if (found != std::string::npos)
			{
				displayName = userScriptFullName.substr(found + 1);
			}

			InitializeDrawers(scriptComponent);
		}
	}

	void UserScriptInspector::Draw()
	{
		if (ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const auto& drawer : drawers)
				drawer->Draw();
		}
	}

	void UserScriptInspector::UpdateFields()
	{
		drawers.clear();
		if (ScriptComponent* userScript = m_GameObject.TryGetComponent<ScriptComponent>())
		{
			InitializeDrawers(userScript);
		}
	}

	void UserScriptInspector::InitializeDrawers(ScriptComponent* userScript)
	{
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		Coral::Type type = userScript->GetType();
		Coral::ManagedObject userObject = userScript->GetManagedObject();

		std::vector<Coral::FieldInfo> fields = type.GetFields();

		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::Entity)
			{
				auto drawer = std::make_shared<EntityFieldDrawer>(fieldStorage.Name, 0,
					[](GUID guid) {  });
				drawers.push_back(drawer);
			}
			else if (fieldStorage.DataType == DataType::String)
				CreateStringDrawer(m_GameObject, fieldStorage.Name, userObject);
			else
				CreateDrawerFromProperty(m_GameObject, fieldStorage.Name, fieldStorage.Type->GetManagedType(), userObject);
		}
	}

	void UserScriptInspector::CreateDrawerFromProperty(GameObject& gameObject, const std::string& fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject)
	{
		switch (managedType)
		{
		case Coral::ManagedType::SByte:
		{
			char8_t intialValue = userObject.GetFieldValue<char8_t>(fieldName);
			auto drawer = AddIntDrawer<char8_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Byte:
		{
			uint8_t intialValue = userObject.GetFieldValue<uint8_t>(fieldName);
			auto drawer = AddIntDrawer<uint8_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Short:
		{
			int16_t intialValue = userObject.GetFieldValue<uint16_t>(fieldName);
			auto drawer = AddIntDrawer<int16_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::UShort:
		{
			uint16_t intialValue = userObject.GetFieldValue<uint16_t>(fieldName);
			auto drawer = AddIntDrawer<uint16_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::UInt:
		{
			uint32_t intialValue = userObject.GetFieldValue<uint32_t>(fieldName);
			auto drawer = AddIntDrawer<uint32_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Long:
		{
			uint64_t intialValue = userObject.GetFieldValue<uint64_t>(fieldName);
			auto drawer = AddIntDrawer<uint64_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::ULong:
		{
			uint64_t intialValue = userObject.GetFieldValue<uint64_t>(fieldName);
			auto drawer = AddIntDrawer<uint64_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Int:
		{
			int32_t intialValue = userObject.GetFieldValue<int32_t>(fieldName);
			auto drawer = AddIntDrawer<int32_t>(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Bool:
		{
			bool initialValue = userObject.GetFieldValue<uint32_t>(fieldName);
			auto drawer = AddBoolDrawer(gameObject, fieldName, initialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Double:
		{
			double intialValue = userObject.GetFieldValue<double>(fieldName);
			auto drawer = AddDoubleDrawer(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		case Coral::ManagedType::Float:
		{
			float intialValue = userObject.GetFieldValue<float>(fieldName);
			auto drawer = AddFloatDrawer(gameObject, fieldName, intialValue);
			drawers.push_back(drawer);
			break;
		}
		}
	}

	void UserScriptInspector::CreateStringDrawer(GameObject& gameObject, const std::string& fieldName, Coral::ManagedObject userObject)
	{
		Coral::ScopedString initialValue = userObject.GetFieldValue<Coral::String>(fieldName);
		AddStringDrawer(gameObject, fieldName, initialValue);
	}
}