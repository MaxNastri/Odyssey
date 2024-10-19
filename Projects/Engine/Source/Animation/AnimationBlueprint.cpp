#include "AnimationBlueprint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "widgets.h"
#include "AnimationNodes.h"
#include "AnimationProperty.hpp"
#include "Input.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint()
	{
		m_Builder = std::make_unique<BlueprintBuilder>(this);
		m_Builder->OverrideCreateNodeMenu(Create_Node_Menu, m_CreateNodeMenuID);

		AddNode<AnimationStateNode>("Idle");
		AddNode<AnimationStateNode>("Run");

		m_Properties.emplace_back(std::make_shared<AnimationProperty>("Jumping", AnimationPropertyType::Bool));
		m_Properties.emplace_back(std::make_shared<AnimationProperty>("Speed", AnimationPropertyType::Float));
		m_Properties.emplace_back(std::make_shared<AnimationProperty>("Hit Count", AnimationPropertyType::Int));
		m_Properties.emplace_back(std::make_shared<AnimationProperty>("Swing Weapon", AnimationPropertyType::Trigger));

		for (auto& animProperty : m_Properties)
		{
			m_PropertyMap[animProperty->Name] = animProperty;
		}

		// Build nodes
		BuildNodes();
	}

	void AnimationBlueprint::Update()
	{
		ClearTriggers();
	}

	void AnimationBlueprint::Draw()
	{
		m_Builder->SetEditor();

		DrawPropertiesPanel();

		ImGui::SameLine(0.0f, 12.0f);

		// Begin building the UI
		m_Builder->Begin();

		// Draw the blueprint
		m_Builder->DrawBlueprint();

		// End building the UI
		m_Builder->End();

		// Override the create node menu
		ImGui::PushOverrideID(m_CreateNodeMenuID);
		if (ImGui::BeginPopup(Create_Node_Menu.c_str()))
		{
			ImGui::TextUnformatted("Create New Node");
			ImGui::Separator();

			std::shared_ptr<Node> node;

			if (ImGui::MenuItem("Animation State"))
				node = AddNode<AnimationStateNode>("State");

			if (node)
				m_Builder->ConnectNewNode(node.get());

			ImGui::EndPopup();
		}
		ImGui::PopID();

		ImGui::PushOverrideID(m_AddLinkMenuID);
		if (ImGui::BeginPopup("Add Link Menu"))
		{
			static std::string name = "Hello";

			ImGui::Text("Edit name:");
			ImGui::InputText("##edit", name.data(), name.size());
			if (ImGui::Button("Apply"))
			{
				Blueprint::AddLink(m_PendingLinkStart, m_PendingLinkEnd);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void AnimationBlueprint::AddLink(Pin* start, Pin* end)
	{
		m_PendingLinkStart = start;
		m_PendingLinkEnd = end;
		ImGui::PushOverrideID(m_AddLinkMenuID);
		ImGui::OpenPopup("Add Link Menu");
		ImGui::PopID();
	}

	bool AnimationBlueprint::SetBool(const std::string& name, bool value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetFloat(const std::string& name, float value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetInt(const std::string& name, int32_t value)
	{
		if (m_PropertyMap.contains(name))
		{
			m_PropertyMap[name]->ValueBuffer.Write(&value);
			return true;
		}
		return false;
	}

	bool AnimationBlueprint::SetTrigger(const std::string& name)
	{
		if (m_PropertyMap.contains(name))
		{
			bool trigger = true;
			m_PropertyMap[name]->ValueBuffer.Write(&trigger);
			return true;
		}
		return false;
	}

	void AnimationBlueprint::ClearTriggers()
	{
		const bool clearTrigger = false;

		for (auto& property : m_Properties)
		{
			if (property->Type == AnimationPropertyType::Trigger)
			{
				property->ValueBuffer.Write(&clearTrigger);
			}
		}
	}

	void AnimationBlueprint::EvalulateGraph()
	{

	}

	void AnimationBlueprint::DrawPropertiesPanel()
	{
		constexpr float2 buttonSize = float2(25.0f, 25.0f);
		constexpr float splitPadding = 4.0f;

		ImGui::Widgets::Splitter(true, splitPadding, m_PropertiesPanel.Size, m_PropertiesPanel.MinSize);

		ImGui::BeginChild("Properties", float2(m_PropertiesPanel.Size.x - splitPadding, 0.0f));
		ImGui::BeginHorizontal("Property Editor");

		float panelWidth = ImGui::GetContentRegionAvail().x;
		float panelLeft = ImGui::GetCursorPosX();
		auto style = ImGui::GetStyle();

		if (ImGui::BeginPopup("Add Property Menu"))
		{
			ImGui::MenuItem("Float");
			ImGui::MenuItem("Int");
			ImGui::MenuItem("Bool");
			ImGui::MenuItem("Trigger");
			ImGui::EndPopup();
		}

		// + button
		float2 addButtonSize = ImGui::CalcTextSize("+") + style.FramePadding + style.ItemSpacing;
		float addButtonPosition = panelWidth - addButtonSize.x - style.FramePadding.x;

		ImGui::SetCursorPosX(addButtonPosition);
		if (ImGui::Button("+", addButtonSize))
			ImGui::OpenPopup("Add Property Menu");

		// Navigate button
		float2 navButtonSize = ImGui::CalcTextSize("Navigate") + style.FramePadding + style.ItemSpacing;
		float navButtonPos = addButtonPosition - navButtonSize.x - style.ItemSpacing.x;

		ImGui::SetCursorPosX(navButtonPos);
		if (ImGui::Button("Navigate", navButtonSize))
			m_Builder->NavigateToContent();

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
		// Table (2 columns like property drawers)
		// Re-orderable selections (see Demo)

		float labelWidth = 0.0f;

		for (size_t i = 0; i < m_Properties.size(); i++)
		{
			float2 nameSize = ImGui::CalcTextSize(m_Properties[i]->Name.c_str());
			labelWidth = std::max(labelWidth, nameSize.x);
		}

		labelWidth = (labelWidth + style.FramePadding.x + style.ItemSpacing.x) / panelWidth;

		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingMask_))
		{
			ImGui::TableSetupColumn("##empty", 0, labelWidth);

			for (size_t i = 0; i < m_Properties.size(); i++)
			{
				auto& animProperty = m_Properties[i];

				// Name column
				ImGui::TableNextColumn();
				ImGui::Selectable(animProperty->Name.c_str());

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					int32_t direction = Input::GetMouseAxisVertical() > 0.0f ? 1 : -1;
					size_t next = i + direction;
				
					if (next >= 0 && next < m_Properties.size())
					{
						std::swap(m_Properties[i], m_Properties[next]);
					}
				}

				// Widget column
				ImGui::TableNextColumn();

				float inputWidth = 100.0f;
				float position = panelWidth - inputWidth - style.FramePadding.x;
				ImGui::SetCursorPosX(position);
				ImGui::SetNextItemWidth(inputWidth);

				ImGui::PushID(i);
				switch (animProperty->Type)
				{
					case AnimationPropertyType::Float:
					{
						float data = animProperty->ValueBuffer.Read<float>();
						
						if (ImGui::InputFloat("##label", &data))
							animProperty->ValueBuffer.Write(&data, sizeof(float));

						break;
					}
					case AnimationPropertyType::Int:
					{
						int32_t data = animProperty->ValueBuffer.Read<int32_t>();

						if (ImGui::InputScalar("##label", ImGuiDataType_S32, &data))
							animProperty->ValueBuffer.Write(&data, sizeof(int32_t));

						break;
					}
					case AnimationPropertyType::Bool:
					{
						bool data = animProperty->ValueBuffer.Read<bool>();

						if (ImGui::Checkbox("##label", &data))
							animProperty->ValueBuffer.Write(&data, sizeof(bool));

						break;
					}
					case AnimationPropertyType::Trigger:
					{
						bool data = animProperty->ValueBuffer.Read<bool>();
						int radio = data;
						if (data = ImGui::RadioButton("##label", &radio, 1))
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

	void AnimationBlueprint::DrawAddAnimationLinkPopup()
	{

	}
}