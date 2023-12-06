#include "UserScriptInspector.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "StringDrawer.h"
#include <UserScript.h>
#include <ComponentManager.h>
#include <imgui.h>
#include "DrawerUtils.h"

namespace Odyssey
{
	UserScriptInspector::UserScriptInspector(GameObject* go, UserScript* userScript, std::string_view className)
	{
		gameObject = go;
		userScriptFullName = displayName = className;

		size_t found = userScriptFullName.find_last_of('.');
		if (found != std::string::npos)
		{
			displayName = userScriptFullName.substr(found + 1);
		}

		InitializeDrawers(userScript);
	}

	void UserScriptInspector::Draw()
	{
		if (ImGui::CollapsingHeader(displayName.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("##UserScriptInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##B", 0, 0.4f);

				for (const auto& drawer : drawers)
				{
					drawer->Draw();
					ImGui::TableNextRow();
				}
				ImGui::EndTable();
			}
		}

		ImGui::Separator();
	}

	void UserScriptInspector::UpdateFields()
	{
		drawers.clear();
		if (UserScript* userScript = ComponentManager::GetUserScript(gameObject->id, userScriptFullName))
		{
			InitializeDrawers(userScript);
		}
	}

	void UserScriptInspector::InitializeDrawers(UserScript* userScript)
	{
		Coral::Type type = userScript->GetType();
		Coral::ManagedObject userObject = userScript->GetManagedObject();

		std::vector<Coral::FieldInfo> fields = type.GetFields();

		for (auto& field : fields)
		{
			if (field.GetAccessibility() == Coral::TypeAccessibility::Public)
			{
				std::string fieldName = field.GetName();
				Coral::Type fieldType = field.GetType();
				bool fieldIsString = fieldType.GetManagedType() == Coral::ManagedType::Unknown && fieldType.GetFullName() == "System.String";

				if (fieldIsString)
				{
					CreateStringDrawer(gameObject, userScriptFullName, fieldName, userObject);
				}
				else
				{
					CreateDrawerFromProperty(gameObject, userScriptFullName, fieldName, field.GetType().GetManagedType(), userObject);
				}
			}
		}
	}

	void UserScriptInspector::CreateDrawerFromProperty(GameObject* gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject)
	{
		switch (managedType)
		{
			case Coral::ManagedType::SByte:
			{
				char8_t intialValue = userObject.GetFieldValue<char8_t>(fieldName);
				AddIntDrawer<char8_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Byte:
			{
				uint8_t intialValue = userObject.GetFieldValue<uint8_t>(fieldName);
				AddIntDrawer<uint8_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Short:
			{
				int16_t intialValue = userObject.GetFieldValue<uint16_t>(fieldName);
				AddIntDrawer<int16_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::UShort:
			{
				uint16_t intialValue = userObject.GetFieldValue<uint16_t>(fieldName);
				AddIntDrawer<uint16_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::UInt:
			{
				uint32_t intialValue = userObject.GetFieldValue<uint32_t>(fieldName);
				AddIntDrawer<uint32_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Long:
			{
				uint64_t intialValue = userObject.GetFieldValue<uint64_t>(fieldName);
				AddIntDrawer<uint64_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::ULong:
			{
				uint64_t intialValue = userObject.GetFieldValue<uint64_t>(fieldName);
				AddIntDrawer<uint64_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Int:
			{
				int32_t intialValue = userObject.GetFieldValue<int32_t>(fieldName);
				AddIntDrawer<int32_t>(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Bool:
			{
				bool initialValue = userObject.GetFieldValue<uint32_t>(fieldName);
				AddBoolDrawer(gameObject, className, fieldName, initialValue, drawers);
				break;
			}
			case Coral::ManagedType::Double:
			{
				double intialValue = userObject.GetFieldValue<double>(fieldName);
				AddDoubleDrawer(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
			case Coral::ManagedType::Float:
			{
				float intialValue = userObject.GetFieldValue<float>(fieldName);
				AddFloatDrawer(gameObject, className, fieldName, intialValue, drawers);
				break;
			}
		}
	}

	void UserScriptInspector::CreateStringDrawer(GameObject* gameObject, const std::string& className, const std::string& fieldName, Coral::ManagedObject userObject)
	{
		Coral::ScopedString initialValue = userObject.GetFieldValue<Coral::String>(fieldName);
		AddStringDrawer(gameObject, className, fieldName, initialValue, drawers);
	}
}