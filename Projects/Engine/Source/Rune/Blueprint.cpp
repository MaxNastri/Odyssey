#include "Blueprint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include "BlueprintBuilder.h"
#include "Enum.hpp"

namespace Odyssey::Rune
{
	Blueprint::Blueprint()
		: m_Name("Blueprint"), m_Builder(this)
	{
		m_Builder.OverrideCreateNodeMenu(Create_Node_Menu, m_CreateNodeMenuID);

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

	void Blueprint::Update()
	{
		m_Builder.DrawBlueprint();

		ImGui::PushOverrideID(m_CreateNodeMenuID);
		if (ImGui::BeginPopup(Create_Node_Menu.c_str()))
		{
			ImGui::TextUnformatted("Create New Node");
			ImGui::Separator();

			std::shared_ptr<Node> node;

			if (ImGui::MenuItem("Branch"))
				node = AddNode<BranchNode>("Branch");
			if (ImGui::MenuItem("Simple"))
				node = AddNode<SimpleNode>("Simple");
			if (ImGui::MenuItem("Group"))
				node = AddNode<GroupNode>("Group");

			if (node)
				m_Builder.ConnectNewNode(node.get());

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void Blueprint::AddLink(Pin* start, Pin* end)
	{
		start->Linked = true;
		end->Linked = true;

		Link& newLink = m_Links.emplace_back(Link(start->ID, end->ID));
		newLink.Color = start->GetColor();
	}

	void Blueprint::DeleteNode(NodeId nodeID)
	{
		auto foundNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeID](auto& node) { return node->ID == nodeID; });
		if (foundNode != m_Nodes.end())
			m_Nodes.erase(foundNode);
	}

	void Blueprint::DeleteLink(LinkId linkID)
	{
		auto foundID = std::find_if(m_Links.begin(), m_Links.end(), [linkID](Link& link) { return link.ID == linkID; });
		if (foundID != m_Links.end())
		{
			if (Pin* startPin = FindPin((*foundID).StartPinID))
				startPin->Linked = false;

			if (Pin* endPin = FindPin((*foundID).EndPinID))
				endPin->Linked = false;

			m_Links.erase(foundID);
		}
	}

	Node* Blueprint::FindNode(NodeId nodeID)
	{
		for (auto& node : m_Nodes)
		{
			if (node->ID == nodeID)
				return node.get();
		}

		return nullptr;
	}

	Link* Blueprint::FindLink(LinkId linkID)
	{
		for (Link& link : m_Links)
		{
			if (link.ID == linkID)
				return &link;
		}

		return nullptr;
	}

	Pin* Blueprint::FindPin(PinId pinID)
	{
		if (!pinID)
			return nullptr;

		for (auto& node : m_Nodes)
		{
			for (Pin& input : node->Inputs)
			{
				if (input.ID == pinID)
					return &input;
			}

			for (Pin& output : node->Outputs)
			{
				if (output.ID == pinID)
					return &output;
			}
		}

		return nullptr;
	}

	void Blueprint::BuildNodes()
	{
		for (auto& node : m_Nodes)
			BuildNode(node.get());
	}

	void Blueprint::BuildNode(Node* node)
	{
		for (auto& input : node->Inputs)
		{
			input.Node = node;
			input.IO = PinIO::Input;
		}

		for (auto& output : node->Outputs)
		{
			output.Node = node;
			output.IO = PinIO::Output;
		}
	}

	void Blueprint::BreakLinks(Pin* pin)
	{
		std::vector<size_t> removals;

		for (size_t i = 0; i < m_Links.size(); i++)
		{
			Link& link = m_Links[i];

			if (link.StartPinID == pin->ID || link.EndPinID == pin->ID)
			{
				Pin* start = FindPin(link.StartPinID);
				Pin* end = FindPin(link.EndPinID);

				start->Linked = false;
				end->Linked = false;

				removals.push_back(i);
			}
		}

		// Reverse so we go from back to front while erasing
		for (int32_t i = removals.size() - 1; i >= 0; i--)
			m_Links.erase(m_Links.begin() + removals[i]);
	}

	size_t Blueprint::LoadNodeSettings(NodeId nodeId, char* data)
	{
		auto node = FindNode(nodeId);

		if (!node)
			return 0;

		// Copy the state into the node
		if (data != nullptr)
			memcpy(data, node->State.data(), node->State.size());

		return node->State.size();
	}

	bool Blueprint::SaveNodeSettings(NodeId nodeId, const char* data, size_t size)
	{
		// Find the node
		auto node = FindNode(nodeId);

		if (!node)
			return false;

		// Assign the node's state
		node->State.assign(data, size);

		return true;
	}
}