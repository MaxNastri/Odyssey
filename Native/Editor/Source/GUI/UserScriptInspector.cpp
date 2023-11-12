#include "UserScriptInspector.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "StringDrawer.h"
#include <UserScript.h>
#include <ComponentManager.h>
#include <imgui.h>

namespace Odyssey::Editor
{
	template <typename FieldType>
	void AddIntDrawer(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedObject userObject, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		using namespace Entities;

		FieldType fieldValue = userObject.GetFieldValue<FieldType>(fieldName);
		std::function<void(FieldType)> callback = [gameObject, fieldName](FieldType fieldValue)
			{
				if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
				{
					userScript->GetManagedObject().SetFieldValue<FieldType>(fieldName, fieldValue);
				}
			};

		std::unique_ptr<IntDrawer<FieldType>> drawer = std::make_unique<IntDrawer<FieldType>>(fieldName, fieldValue, callback);
		drawers.push_back(std::move(drawer));
	}

	UserScriptInspector::UserScriptInspector(Entities::GameObject go)
	{
		gameObject = go;
		InitializeDrawers();
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

	void UserScriptInspector::UpdateFields()
	{
		drawers.clear();
		InitializeDrawers();
	}

	
	void UserScriptInspector::InitializeDrawers()
	{
		using namespace Entities;

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
					Coral::Type fieldType = field.GetType();
					if (fieldType.IsString())
					{
						CreateStringDrawer(gameObject, fieldName, userObject);
					}
					else
					{
						CreateDrawerFromProperty(gameObject, fieldName, field.GetType().GetManagedType(), userObject);
					}
				}
			}
		}
	}

	void UserScriptInspector::CreateDrawerFromProperty(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedType managedType, Coral::ManagedObject userObject)
	{
		using namespace Entities;

		switch (managedType)
		{
			case Coral::ManagedType::SByte:
			{
				AddIntDrawer<char8_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::Byte:
			{
				AddIntDrawer<uint8_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::Short:
			{
				AddIntDrawer<uint16_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::UShort:
			{
				AddIntDrawer<uint16_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::UInt:
			{
				AddIntDrawer<uint32_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::Long:
			{
				AddIntDrawer<uint64_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::ULong:
			{
				AddIntDrawer<uint64_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::Int:
			{
				AddIntDrawer<int32_t>(gameObject, fieldName, userObject, drawers);
				break;
			}
			case Coral::ManagedType::Bool:
			{
				std::function<void(bool)> callback =
					[gameObject, fieldName](bool fieldValue)
					{
						if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetFieldValue<uint32_t>(fieldName, fieldValue);
						}
					};

				std::unique_ptr<BoolDrawer> drawer = std::make_unique<BoolDrawer>(fieldName, userObject.GetFieldValue<uint32_t>(fieldName), callback);
				drawers.push_back(std::move(drawer));
				break;
			}
			case Coral::ManagedType::Double:
			{
				std::function<void(double)> callback = [gameObject, fieldName](double fieldValue)
					{
						if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetFieldValue<double>(fieldName, fieldValue);
						}
					};
				std::unique_ptr<DoubleDrawer> drawer = std::make_unique<DoubleDrawer>(fieldName, userObject.GetFieldValue<double>(fieldName), callback);
				drawers.push_back(std::move(drawer));
				break;
			}
			case Coral::ManagedType::Float:
			{
				std::function<void(float)> callback = [gameObject, fieldName](float fieldValue)
					{
						if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetFieldValue<float>(fieldName, fieldValue);
						}
					};
				std::unique_ptr<FloatDrawer> drawer = std::make_unique<FloatDrawer>(fieldName, userObject.GetFieldValue<float>(fieldName), callback);
				drawers.push_back(std::move(drawer));
				break;
			}
		}
	}

	void UserScriptInspector::CreateStringDrawer(Entities::GameObject gameObject, std::string fieldName, Coral::ManagedObject userObject)
	{
		using namespace Entities;

		std::function<void(std::string)> callback =
			[gameObject, fieldName](std::string fieldValue)
			{
				if (UserScript* userScript = ComponentManager::GetComponent<UserScript>(gameObject))
				{
					Coral::ScopedString field = Coral::String::New(fieldValue);
					userScript->GetManagedObject().SetFieldValue<Coral::String>(fieldName, field);
				}
			};

		Coral::ScopedString field = userObject.GetFieldValue<Coral::String>(fieldName);

		std::unique_ptr<StringDrawer> drawer = std::make_unique<StringDrawer>(fieldName, field, callback);
		drawers.push_back(std::move(drawer));
	}
}