#include "UserScriptInspector.h"
#include "FloatDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include <UserScript.h>
#include <ComponentManager.h>
#include <imgui.h>

namespace Odyssey::Editor
{
	UserScriptInspector::UserScriptInspector(Entities::GameObject go)
	{
		using namespace Entities;
		gameObject = go;

		if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
		{
			Coral::Type type = userScript->GetType();
			Coral::ManagedObject userObject = userScript->GetManagedObject();

			std::vector<Coral::FieldInfo> fields = type.GetFields();

			for (auto& field : fields)
			{
				if (field.GetAccessibility() == Coral::TypeAccessibility::Public)
				{
					std::string fieldName = field.GetName();
					Coral::String typeName = field.GetType().GetFullName();
					if (typeName == "NativeString")
					{
						//Coral::NativeString str = userObject.GetFieldValue<Coral::NativeString>(fieldName);
						int ebug = 0;
					}
					CreateDrawerFromProperty(gameObject, fieldName, field.GetType().GetManagedType(), userObject);
				}
			}
		}
	}

	void UserScriptInspector::Draw()
	{
		if (ImGui::CollapsingHeader("UserScript"))
		{
			if (ImGui::BeginTable("UserScriptInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
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

	void UserScriptInspector::CreateDrawerFromProperty(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject)
	{
		using namespace Entities;

		switch (managedType)
		{
			case Coral::ManagedType::SByte:
				break;
			case Coral::ManagedType::Byte:
				break;
			case Coral::ManagedType::Short:
				break;
			case Coral::ManagedType::UShort:
				break;
			case Coral::ManagedType::UInt:
				break;
			case Coral::ManagedType::Long:
				break;
			case Coral::ManagedType::ULong:
				break;
			case Coral::ManagedType::Int:
			{
				//uint32_t fieldData = userObject.GetFieldValue<uint64_t>(fieldName);

				//std::unique_ptr<IntDrawer> drawer = std::make_unique<IntDrawer>(fieldName, (int)fieldData);
				//drawer->SetCallback
				//(
				//	[gameObject, fieldName](int fieldValue)
				//	{
				//		if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
				//		{
				//			userScript->GetManagedObject().SetPropertyValueRaw(fieldName, &fieldValue);
				//		}
				//	}
				//);
				//drawers.push_back(std::move(drawer));
				break;
			}
			case Coral::ManagedType::Bool:
			{
				std::unique_ptr<BoolDrawer> drawer = std::make_unique<BoolDrawer>(fieldName, userObject.GetFieldValue<uint32_t>(fieldName));
				drawer->SetCallback
				(
					[gameObject, fieldName](bool fieldValue)
					{
						if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetPropertyValue<uint32_t>(fieldName, fieldValue);
						}
					}
				);
				drawers.push_back(std::move(drawer));
				break;
			}
			case Coral::ManagedType::Double:
			case Coral::ManagedType::Float:
			{
				std::unique_ptr<FloatDrawer> drawer = std::make_unique<FloatDrawer>(fieldName, userObject.GetFieldValue<float>(fieldName));
				drawer->SetCallback
				(
					[gameObject, fieldName](float fieldValue)
					{
						if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetPropertyValue<float>(fieldName, fieldValue);
						}
					}
				);
				drawers.push_back(std::move(drawer));
				break;
			}
		}
	}
}