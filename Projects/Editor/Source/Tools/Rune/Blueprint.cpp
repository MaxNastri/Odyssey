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


		m_Nodes.emplace_back(m_NextID++, "InputAction Fire", float4(255, 128, 128, 1.0f));
		m_Nodes.back().Inputs.emplace_back(m_NextID++, "InFlow", PinType::Flow);
		m_Nodes.back().Inputs.emplace_back(m_NextID++, "Input", PinType::Float);
		m_Nodes.back().Outputs.emplace_back(m_NextID++, "OutFlow", PinType::Flow);
		m_Nodes.back().Outputs.emplace_back(m_NextID++, "Output", PinType::Float);

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