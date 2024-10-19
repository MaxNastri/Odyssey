#include "AnimationBlueprintUI.h"
#include "widgets.h"
#include "AnimationBlueprint.h"
#include "AnimationNodes.h"
#include "Input.h"

namespace Odyssey
{
	void PropertiesPanel::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		constexpr float2 buttonSize = float2(25.0f, 25.0f);
		constexpr float splitPadding = 4.0f;

		ImGui::Splitter(true, splitPadding, Size, MinSize);

		ImGui::BeginChild("Properties", float2(Size.x - splitPadding, 0.0f));
		ImGui::BeginHorizontal("Property Editor");

		float panelWidth = ImGui::GetContentRegionAvail().x;
		float panelLeft = ImGui::GetCursorPosX();
		auto style = ImGui::GetStyle();

		// + button
		float2 addButtonSize = ImGui::CalcTextSize("+") + style.FramePadding + style.ItemSpacing;
		float addButtonPosition = panelWidth - addButtonSize.x - style.FramePadding.x;

		ImGui::SetCursorPosX(addButtonPosition);
		if (ImGui::Button("+", addButtonSize))
			blueprintUI.GetSelectPropertyMenu().Open();

		// Navigate button
		float2 navButtonSize = ImGui::CalcTextSize("Navigate") + style.FramePadding + style.ItemSpacing;
		float navButtonPos = addButtonPosition - navButtonSize.x - style.ItemSpacing.x;

		ImGui::SetCursorPosX(navButtonPos);
		ImGui::Button("Navigate", navButtonSize);
		//if (ImGui::Button("Navigate", navButtonSize))
			//m_Builder->NavigateToContent();

		// Reset the cursor to the panel's left and draw the search label
		ImGui::SetCursorPosX(panelLeft);
		ImGui::TextUnformatted("Search");

		// Calculate the width of the search text box
		float currentPos = ImGui::GetCursorPosX();
		float searchWidth = navButtonPos - currentPos - style.ItemSpacing.x;

		// Draw the search text box
		std::string data;
		ImGui::PushItemWidth(searchWidth);
		ImGui::InputText("", data.data(), data.size());
		ImGui::EndHorizontal();

		// Start drawing properties
		float inputWidth = 100.0f;
		float inputPos = panelWidth - inputWidth - style.FramePadding.x;
		float labelWidth = inputPos - style.ItemSpacing.x;

		labelWidth = (labelWidth + style.FramePadding.x + style.ItemSpacing.x) / panelWidth;

		auto& properties = blueprint->GetProperties();

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingMask_))
		{
			for (size_t i = 0; i < properties.size(); i++)
			{
				auto& animProperty = properties[i];

				ImGui::PushID(i);

				// Name column
				ImGui::TableNextColumn();

				char buffer[128] = "";
				animProperty->Name.copy(buffer, 128);

				if (ImGui::SelectableInput("##PropertyLabel", false, ImGuiSelectableFlags_SpanAllColumns, buffer, ARRAYSIZE(buffer)))
					animProperty->Name = buffer;

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					int32_t direction = Input::GetMouseAxisVertical() > 0.0f ? 1 : -1;
					size_t next = i + direction;

					if (next >= 0 && next < properties.size())
					{
						std::swap(properties[i], properties[next]);
					}
				}

				// Widget column
				ImGui::TableNextColumn();

				ImGui::SetCursorPosX(inputPos);
				ImGui::SetNextItemWidth(inputWidth);

				switch (animProperty->Type)
				{
					case AnimationPropertyType::Float:
					{
						float data = animProperty->ValueBuffer.Read<float>();

						if (ImGui::InputFloat("##InputLabel", &data))
							animProperty->ValueBuffer.Write(&data, sizeof(float));

						break;
					}
					case AnimationPropertyType::Int:
					{
						int32_t data = animProperty->ValueBuffer.Read<int32_t>();

						if (ImGui::InputScalar("##InputLabel", ImGuiDataType_S32, &data))
							animProperty->ValueBuffer.Write(&data, sizeof(int32_t));

						break;
					}
					case AnimationPropertyType::Bool:
					{
						bool data = animProperty->ValueBuffer.Read<bool>();

						if (ImGui::Checkbox("##InputLabel", &data))
							animProperty->ValueBuffer.Write(&data, sizeof(bool));

						break;
					}
					case AnimationPropertyType::Trigger:
					{
						bool data = animProperty->ValueBuffer.Read<bool>();
						int radio = data;
						if (data = ImGui::RadioButton("##InputLabel", &radio, 1))
							animProperty->ValueBuffer.Write(&data, sizeof(bool));

						break;
					}
				}
				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	void SelectPropertyMenu::Open()
	{
		ImGui::PushOverrideID(ID);
		ImGui::OpenPopup(Name.c_str());
		ImGui::PopID();
	}

	void SelectPropertyMenu::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			if (ImGui::MenuItem("Float"))
				blueprintUI.GetAddPropertyMenu().Open(AnimationPropertyType::Float);
			if (ImGui::MenuItem("Int"))
				blueprintUI.GetAddPropertyMenu().Open(AnimationPropertyType::Int);
			if (ImGui::MenuItem("Bool"))
				blueprintUI.GetAddPropertyMenu().Open(AnimationPropertyType::Bool);
			if (ImGui::MenuItem("Trigger"))
				blueprintUI.GetAddPropertyMenu().Open(AnimationPropertyType::Trigger);

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AddPropertyMenu::Open(AnimationPropertyType propertyType)
	{
		// Clear the buffer of any previous data
		ZeroMemory(m_Buffer, ARRAYSIZE(m_Buffer));

		// Cache the property type
		PropertyType = propertyType;

		// Open the popup menu
		ImGui::PushOverrideID(ID);
		ImGui::OpenPopup(Name.c_str());
		ImGui::PopID();
	}

	void AddPropertyMenu::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			ImGui::Text("Property Name:");

			// Send focus to the input text by default
			if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !Input::GetMouseButtonDown(MouseButton::Left)))
				ImGui::SetKeyboardFocusHere(0);

			ImGui::InputText("##PropName", m_Buffer, ARRAYSIZE(m_Buffer));

			ImGui::BeginHorizontal(ID);

			if (ImGui::Button("Add") || Input::GetKeyDown(KeyCode::Enter) || Input::GetKeyDown(KeyCode::KeypadEnter))
			{
				blueprint->AddProperty(m_Buffer, PropertyType);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndHorizontal();

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AnimationBlueprintUI::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		m_PropertiesPanel.Draw(blueprint, blueprintUI);
		m_SelectPropertyMenu.Draw(blueprint, blueprintUI);
		m_AddPropertyMenu.Draw(blueprint, blueprintUI);
		m_CreateNodeMenu.Draw(blueprint, blueprintUI);
		m_AddAnimationLinkMenu.Draw(blueprint, blueprintUI);
	}

	void CreateNodeMenu::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			ImGui::TextUnformatted("Create New Node");
			ImGui::Separator();

			std::shared_ptr<Node> node;

			if (ImGui::MenuItem("Animation State"))
				node = blueprint->AddNode<AnimationStateNode>("State");

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AddAnimationLinkMenu::Open()
	{
		// Clear the buffer of any previous data
		ZeroMemory(m_Buffer, ARRAYSIZE(m_Buffer));

		// Open the popup menu
		ImGui::PushOverrideID(ID);
		ImGui::OpenPopup(Name.c_str());
		ImGui::PopID();
	}

	void AddAnimationLinkMenu::Draw(AnimationBlueprint* blueprint, AnimationBlueprintUI& blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			ImGui::Text("Select Property:");

			// Send focus to the input text by default
			if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !Input::GetMouseButtonDown(MouseButton::Left)))
				ImGui::SetKeyboardFocusHere(0);

			ImGui::InputText("##edit", m_Buffer, ARRAYSIZE(m_Buffer));

			if (ImGui::Button("Add") || Input::GetKeyDown(KeyCode::Enter) || Input::GetKeyDown(KeyCode::KeypadEnter))
			{
				blueprint->ConfirmPendingLink();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel") || Input::GetKeyDown(KeyCode::Escape))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
}