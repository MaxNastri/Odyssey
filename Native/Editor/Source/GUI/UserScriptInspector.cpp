#include "UserScriptInspector.h"
#include <UserScript.h>
#include <ComponentManager.h>
#include <vector>

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
			case Coral::ManagedType::Float:
			{
				std::unique_ptr<FloatDrawer> drawer = std::make_unique<FloatDrawer>(fieldName, userObject.GetFieldValue<float>(fieldName));
				drawer->SetCallback
				(
					[gameObject, fieldName](float fieldValue)
					{
						if (Entities::UserScript* userScript = Entities::ComponentManager::GetComponent<Entities::UserScript>(gameObject))
						{
							userScript->GetManagedObject().SetPropertyValue<float>(fieldName, fieldValue);
						}
					}
				);
				drawers.push_back(std::move(drawer));
			}
		}
	}
}