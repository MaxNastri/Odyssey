#include "AnimationBlueprint.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace Odyssey
{
	AnimationBlueprint::AnimationBlueprint()
		: Blueprint()
	{
		m_Builder = std::make_unique<BlueprintBuilder>(this);
		m_Builder->OverrideCreateNodeMenu(Create_Node_Menu, m_CreateNodeMenuID);

		auto& blueprintNode = m_Nodes.emplace_back(std::make_shared<BlueprintNode>("BP Example"));

		auto& bp2Node = m_Nodes.emplace_back(std::make_shared<BlueprintNode>("BP Example 2"));
		bp2Node->Color = float4(0.0f, 1.0f, 0.0f, 1.0f);
		bp2Node->Inputs.emplace_back("Speed", PinType::Float);
		bp2Node->Outputs.emplace_back("YourMom", PinType::Float);

		auto& stringNode = m_Nodes.emplace_back(std::make_shared<SimpleNode>(""));
		stringNode->Outputs[0].Modify("Message", PinType::String);

		auto& treeNode = m_Nodes.emplace_back(std::make_shared<TreeNode>("Tree Example"));;
		auto& groupNode = m_Nodes.emplace_back(std::make_shared<GroupNode>("Group Example"));
		auto& branchNode = m_Nodes.emplace_back(std::make_shared<BlueprintNode>("Branch Example"));
		CreateBranchNode(branchNode);

		// Build nodes
		BuildNodes();
	}

	void AnimationBlueprint::Update()
	{
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
}