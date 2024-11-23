#include "Blueprint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"

namespace Odyssey::Rune
{
	Blueprint::Blueprint()
		: m_Name("Blueprint")
	{
	}

	void Blueprint::AddLink(Pin* start, Pin* end)
	{
		start->Linked = true;
		end->Linked = true;

		Link& newLink = m_Links.emplace_back(Link(start->Guid, end->Guid));
		newLink.Color = start->GetColor();
	}

	void Blueprint::DeleteNode(GUID nodeGUID)
	{
		auto foundNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeGUID](auto& node) { return node->Guid == nodeGUID; });
		if (foundNode != m_Nodes.end())
			m_Nodes.erase(foundNode);
	}

	void Blueprint::DeleteLink(GUID linkGUID)
	{
		auto foundID = std::find_if(m_Links.begin(), m_Links.end(), [linkGUID](Link& link) { return link.Guid == linkGUID; });
		if (foundID != m_Links.end())
		{
			if (Pin* startPin = FindPin((*foundID).StartPinGUID))
				startPin->Linked = false;

			if (Pin* endPin = FindPin((*foundID).EndPinGUID))
				endPin->Linked = false;

			m_Links.erase(foundID);
		}
	}

	Node* Blueprint::FindNode(GUID nodeGUID)
	{
		for (auto& node : m_Nodes)
		{
			if (node->Guid == nodeGUID)
				return node.Get();
		}

		return nullptr;
	}

	Link* Blueprint::FindLink(GUID linkGUID)
	{
		for (Link& link : m_Links)
		{
			if (link.Guid == linkGUID)
				return &link;
		}

		return nullptr;
	}

	Pin* Blueprint::FindPin(GUID pinGUID)
	{
		if (!pinGUID)
			return nullptr;

		for (auto& node : m_Nodes)
		{
			for (Pin& input : node->Inputs)
			{
				if (input.Guid == pinGUID)
					return &input;
			}

			for (Pin& output : node->Outputs)
			{
				if (output.Guid == pinGUID)
					return &output;
			}
		}

		return nullptr;
	}

	void Blueprint::BuildNodes()
	{
		for (auto& node : m_Nodes)
			BuildNode(node.Get());
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

			if (link.StartPinGUID == pin->Guid || link.EndPinGUID == pin->Guid)
			{
				Pin* start = FindPin(link.StartPinGUID);
				Pin* end = FindPin(link.EndPinGUID);

				start->Linked = false;
				end->Linked = false;

				removals.push_back(i);
			}
		}

		// Reverse so we go from back to front while erasing
		for (int32_t i = removals.size() - 1; i >= 0; i--)
			m_Links.erase(m_Links.begin() + removals[i]);
	}

	void Blueprint::AddLink(GUID linkGUID, GUID beginGUID, GUID endGUID)
	{
		Node* beginNode = FindNode(beginGUID);
		Node* endNode = FindNode(endGUID);

		if (beginNode && endNode)
		{
			Pin* beginPin = &beginNode->Outputs[0];
			Pin* endPin = &endNode->Inputs[0];
			beginPin->Linked = true;
			endPin->Linked = true;

			Link& newLink = m_Links.emplace_back(Link(linkGUID, beginPin->Guid, endPin->Guid));
			newLink.Color = beginPin->GetColor();
		}
	}

	//size_t Blueprint::LoadNodeSettings(NodeID nodeId, char* data)
	//{
	//	auto node = FindNode(nodeId);

	//	if (!node)
	//		return 0;

	//	// Copy the state into the node
	//	if (data != nullptr)
	//		memcpy(data, node->State.data(), node->State.size());

	//	return node->State.size();
	//}

	//bool Blueprint::SaveNodeSettings(NodeID nodeId, const char* data, size_t size)
	//{
	//	// Find the node
	//	auto node = FindNode(nodeId);

	//	if (!node)
	//		return false;

	//	// Assign the node's state
	//	node->State.assign(data, size);

	//	return true;
	//}
}