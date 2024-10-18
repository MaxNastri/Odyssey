#include "AnimationBlueprint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "widgets.h"
#include "AnimationNodes.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint()
	{
		m_Builder = std::make_unique<BlueprintBuilder>(this);
		m_Builder->OverrideCreateNodeMenu(Create_Node_Menu, m_CreateNodeMenuID);

		AddNode<AnimationStateNode>("Idle");

		// Build nodes
		BuildNodes();
	}

	void AnimationBlueprint::Update()
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
				node = AddNode<BranchNode>("Branch");

			if (node)
				m_Builder->ConnectNewNode(node.get());

			ImGui::EndPopup();
		}
		ImGui::PopID();

	}

	bool AnimationBlueprint::SetProperty(const std::string& name, bool value)
	{
		if (!m_Properties.contains(name))
			m_Properties[name].ValueBuffer.Allocate(sizeof(value));

		RawBuffer& valueBuffer = m_Properties[name].ValueBuffer;
		valueBuffer.Write(&value, sizeof(value));

		return true;
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
		ImGui::PushItemWidth(searchWidth );
		ImGui::InputText("", data.data(), data.size());
		ImGui::EndHorizontal();

		// Start drawing properties
		// Table (2 columns like property drawers)
		// Re-orderable selections (see Demo)

		ImGui::EndChild();
	}
}