#include "Blueprint.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "RuneUIBuilder.h"

namespace Odyssey::Rune
{
	Blueprint::Blueprint()
	{
		// Init the node editor
		ImguiExt::Config config;
		config.SettingsFile = "Blueprints.json";
		config.UserPointer = this;
		config.LoadNodeSettings = [](ImguiExt::NodeId nodeId, char* data, void* userPointer)
			{
				Blueprint* blueprint = static_cast<Blueprint*>(userPointer);
				return blueprint->LoadNodeSettings(nodeId.Get(), data);
			};
		config.SaveNodeSettings = [](ImguiExt::NodeId nodeId, const char* data, size_t size, ImguiExt::SaveReasonFlags reason, void* userPointer)
			{
				Blueprint* blueprint = static_cast<Blueprint*>(userPointer);
				return blueprint->SaveNodeSettings(nodeId.Get(), data, size);
			};

		m_Context = ImguiExt::CreateEditor(&config);
		ImguiExt::SetCurrentEditor(m_Context);


		Node& bpNode = m_Nodes.emplace_back(m_NextID++, "BP Example", float4(1.0f, 0.5f, 0.5f, 1.0f));
		bpNode.Inputs.emplace_back(m_NextID++, "InFlow", PinType::Flow);
		bpNode.Inputs.emplace_back(m_NextID++, "Input", PinType::Float);
		bpNode.Outputs.emplace_back(m_NextID++, "OutFlow", PinType::Flow);
		bpNode.Outputs.emplace_back(m_NextID++, "Output", PinType::Float);

		Node& branchNode = m_Nodes.emplace_back(m_NextID++, "Branch Example", float4(1.0f, 1.0f, 1.0f, 1.0f));
		branchNode.Type = NodeType::Tree;
		branchNode.Inputs.emplace_back(m_NextID++, "", PinType::Flow);

		// Build nodes
		BuildNodes();

		// Navigate to the node editor
		ImguiExt::NavigateToContent();
	}

	Blueprint::~Blueprint()
	{
		if (m_Context)
		{
			ImguiExt::DestroyEditor(m_Context);
			m_Context = nullptr;
		}
	}

	void Blueprint::Update()
	{
		ImguiExt::SetCurrentEditor(m_Context);

		ImguiExt::Begin("Blueprint");

		RuneUIBuilder builder;
		builder.DrawNode(&m_Nodes[0]);
		builder.DrawNode(&m_Nodes[1]);
		ImguiExt::End();
	}

	void Blueprint::BuildNodes()
	{
		for (Node& node : m_Nodes)
			BuildNode(&node);
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

	Node* Blueprint::FindNode(NodeId nodeID)
	{
		for (Node& node : m_Nodes)
		{
			if (node.ID == nodeID)
				return &node;
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

		// Assign the node's state and touch it
		node->State.assign(data, size);
		//TouchNode(nodeId);

		return true;
	}
}