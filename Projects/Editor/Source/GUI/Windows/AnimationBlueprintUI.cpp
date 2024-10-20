#include "AnimationBlueprintUI.h"
#include "imgui.hpp"
#include "AnimationNodes.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "Input.h"
#include "Enum.hpp"

namespace Odyssey
{
	void PropertiesPanel::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
	{
		constexpr float2 buttonSize = float2(25.0f, 25.0f);

		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		if (ImGui::Begin(Window_Name.c_str()))
		{
			float2 textSize = ImGui::CalcTextSize("Property Editor") * 1.1f;
			ImGui::FilledRectSpanText("Property Editor", float4(1.0f), float4(0.15f, 0.15f, 0.15f, 1.0f), textSize.y, float2(1.0f, 0.0f));

			ImGui::BeginHorizontal("Property Editor");
			float panelWidth = ImGui::GetContentRegionAvail().x;
			float panelLeft = ImGui::GetCursorPosX();
			auto style = ImGui::GetStyle();

			// + button
			float2 addButtonSize = ImGui::CalcTextSize("+") + style.FramePadding + style.ItemSpacing;
			float addButtonPosition = panelWidth - addButtonSize.x - style.FramePadding.x;

			ImGui::SetCursorPosX(addButtonPosition);
			if (ImGui::Button("+", addButtonSize))
				blueprintUI->GetSelectPropertyMenu().Open();

			// Navigate button
			float2 navButtonSize = ImGui::CalcTextSize("Navigate") + style.FramePadding + style.ItemSpacing;
			float navButtonPos = addButtonPosition - navButtonSize.x - style.ItemSpacing.x;

			ImGui::SetCursorPosX(navButtonPos);
			if (ImGui::Button("Navigate", navButtonSize))
				ImguiExt::NavigateToContent(false);

			// Reset the cursor to the panel's left and draw the search label
			ImGui::SetCursorPosX(panelLeft);
			ImGui::TextUnformatted("Search");

			// Calculate the width of the search text box
			float currentPos = ImGui::GetCursorPosX();
			float searchWidth = navButtonPos - currentPos - style.ItemSpacing.x;

			// Draw the search text box
			std::string data;
			ImGui::PushItemWidth(searchWidth);
			ImGui::InputText("##SearchLabel", data.data(), data.size());
			ImGui::EndHorizontal();

			// Start drawing properties
			float inputWidth = 100.0f;
			float inputPos = panelWidth - inputWidth - style.FramePadding.x;
			float labelWidth = inputPos - style.ItemSpacing.x;

			labelWidth = (labelWidth + style.FramePadding.x + style.ItemSpacing.x) / panelWidth;

			auto& properties = blueprint->GetProperties();

			if (ImGui::BeginTable("##PropertyTable", 2, ImGuiTableFlags_SizingMask_))
			{
				for (size_t i = 0; i < properties.size(); i++)
				{
					auto& animProperty = properties[i];

					ImGui::PushID((int32_t)i);

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

			// Draw the node inspector panel within the properties panel
			blueprintUI->GetNodeInspectorPanel().Draw(blueprint, builder, blueprintUI);

			ImGui::End();
		}
	}

	void NodeInspectorPanel::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
	{
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window_class);

		static bool menu = true;

		if (ImGui::Begin(Window_Name.c_str()))
		{
			float2 textSize = ImGui::CalcTextSize("Hello world") * 1.1f;
			ImGui::FilledRectSpanText("Node Inspector", float4(1.0f), float4(0.15f, 0.15f, 0.15f, 1.0f), textSize.y, float2(1.0f, 0.0f));

			if (ImguiExt::HasSelectionChanged())
			{
				ImguiExt::NodeId node;
				ImguiExt::GetSelectedNodes(&node, 1);
				m_AnimationState = blueprint->GetAnimationState(node.Get());
				m_AnimationClipDrawer = AssetFieldDrawer("Animation Clip", m_AnimationClipDrawer.GetGUID(), AnimationClip::Type,
					[this](GUID guid) { OnAnimationClipChanged(guid); });
			}

			if (m_AnimationState)
			{
				ImGui::TextUnformatted(m_AnimationState->GetName().data());
				m_AnimationClipDrawer.Draw();
			}
			ImGui::End();
		}
	}

	void NodeInspectorPanel::OnAnimationClipChanged(GUID guid)
	{
		if (m_AnimationState)
			m_AnimationState->SetClip(guid);
	}

	void SelectPropertyMenu::Open()
	{
		ImGui::PushOverrideID(ID);
		ImGui::OpenPopup(Name.c_str());
		ImGui::PopID();
	}

	void SelectPropertyMenu::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			if (ImGui::MenuItem("Float"))
				blueprintUI->GetAddPropertyMenu().Open(AnimationPropertyType::Float);
			if (ImGui::MenuItem("Int"))
				blueprintUI->GetAddPropertyMenu().Open(AnimationPropertyType::Int);
			if (ImGui::MenuItem("Bool"))
				blueprintUI->GetAddPropertyMenu().Open(AnimationPropertyType::Bool);
			if (ImGui::MenuItem("Trigger"))
				blueprintUI->GetAddPropertyMenu().Open(AnimationPropertyType::Trigger);

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

	void AddPropertyMenu::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
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

	void AnimationBlueprintUI::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder)
	{
		m_PropertiesPanel.Draw(blueprint, builder, this);
		m_SelectPropertyMenu.Draw(blueprint, builder, this);
		m_AddPropertyMenu.Draw(blueprint, builder, this);
		m_CreateNodeMenu.Draw(blueprint, builder, this);
		m_AddAnimationLinkMenu.Draw(blueprint, builder, this);
	}

	void CreateNodeMenu::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			ImGui::TextUnformatted("Create New Node");
			ImGui::Separator();

			std::shared_ptr<Node> node;

			if (ImGui::MenuItem("Animation State"))
			{
				node = blueprint->AddNode<AnimationStateNode>("State");
				builder->ConnectNewNode(node.get());
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AddAnimationLinkMenu::Open()
	{
		// Clear any previous data
		Clear();

		// Open the popup menu
		ImGui::PushOverrideID(ID);
		ImGui::OpenPopup(Name.c_str());
		ImGui::PopID();
	}

	void AddAnimationLinkMenu::Draw(AnimationBlueprint* blueprint, BlueprintBuilder* builder, AnimationBlueprintUI* blueprintUI)
	{
		ImGui::PushOverrideID(ID);

		if (ImGui::BeginPopup(Name.c_str()))
		{
			ImGui::Text("Select Property:");

			auto& properties = blueprint->GetProperties();
			const std::string display = m_SelectedProperty >= 0 ? properties[m_SelectedProperty]->Name : "";

			if (ImGui::BeginCombo("##PropertyCombo", display.c_str()))
			{
				for (size_t i = 0; i < properties.size(); i++)
				{
					auto& animProperty = properties[i];

					const bool selected = m_SelectedProperty == i;
					const std::string& name = animProperty->Name;

					if (ImGui::Selectable(name.c_str(), selected))
					{
						m_SelectedProperty = (int32_t)i;
						m_InputValue.Allocate(sizeof(animProperty->ValueBuffer.GetSize()));

						// Triggers will always compare against true, so write it by default
						if (animProperty->Type == AnimationPropertyType::Trigger)
						{
							const bool trigger = true;
							m_InputValue.Write(&trigger);
							m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);
						}
						else if (animProperty->Type == AnimationPropertyType::Bool)
						{
							// Assign a default comparison op of Equal for bools
							m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);
						}
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Only show the comparison op dropdown when a valid property is selected
			// Skip the comparison op UI for triggers
			if (m_SelectedProperty >= 0 && properties[m_SelectedProperty]->Type != AnimationPropertyType::Trigger)
			{
				// For bools always force the comparison op to Equals
				if (properties[m_SelectedProperty]->Type == AnimationPropertyType::Bool)
					m_SelectedComparisonOp = Enum::ToInt(ComparisonOp::Equal);

				// Get the display string
				const std::string comparisonOp = m_SelectedComparisonOp >= 0 ?
					Comparison_Op_Display[m_SelectedComparisonOp] : "";

				if (ImGui::BeginCombo("##CompareOpCombo", comparisonOp.c_str()))
				{
					// Skip the dropdown selection for bools
					if (properties[m_SelectedProperty]->Type != AnimationPropertyType::Bool)
					{
						for (size_t i = 0; i < Comparison_Op_Display.size(); i++)
						{
							const bool selected = m_SelectedComparisonOp == i;
							std::string_view display = Comparison_Op_Display[i];

							if (ImGui::Selectable(display.data(), selected))
								m_SelectedComparisonOp = (int32_t)i;
						}
					}

					ImGui::EndCombo();
				}
			}

			if (m_SelectedProperty >= 0 && m_SelectedComparisonOp >= 0)
			{
				auto& animProperty = properties[m_SelectedProperty];

				switch (animProperty->Type)
				{
					case AnimationPropertyType::Float:
					{
						float data = m_InputValue.Read<float>();

						if (ImGui::InputFloat("##InputLabel", &data))
							m_InputValue.Write(&data);

						break;
					}
					case AnimationPropertyType::Int:
					{
						int32_t data = m_InputValue.Read<int32_t>();

						if (ImGui::InputScalar("##InputLabel", ImGuiDataType_S32, &data))
							m_InputValue.Write(&data);

						break;
					}
					case AnimationPropertyType::Bool:
					{
						bool data = m_InputValue.Read<bool>();
						const std::string boolValue = data ? "True" : "False";

						if (ImGui::BeginCombo("##BoolCombo", boolValue.c_str()))
						{
							if (ImGui::Selectable("True", data))
							{
								const bool trueValue = true;
								m_InputValue.Write(&trueValue);
							}
							if (ImGui::Selectable("False", !data))
							{
								const bool falseValue = false;
								m_InputValue.Write(&falseValue);
							}

							ImGui::EndCombo();
						}

						break;
					}
				}

				// Only allow the add button when the other 2 fields are filled
				if (ImGui::Button("Add") || Input::GetKeyDown(KeyCode::Enter) || Input::GetKeyDown(KeyCode::KeypadEnter))
				{
					// builder->connect pending link
					builder->ConfirmPendingLink();
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
			}

			if (ImGui::Button("Cancel") || Input::GetKeyDown(KeyCode::Escape))
			{
				builder->ClearPendingLink();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	void AddAnimationLinkMenu::Clear()
	{
		ZeroMemory(m_Buffer, ARRAYSIZE(m_Buffer));
		m_SelectedProperty = -1;
		m_SelectedComparisonOp = -1;
		m_InputValue.Free();
	}
}