#include "Blueprint.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "RuneUIBuilder.h"

namespace Odyssey::Rune
{
	namespace ed = ax::NodeEditor;

	Blueprint::Blueprint()
		: m_Name("Blueprint")
	{
		InitNodeEditor();

		auto& blueprintNode = m_Nodes.emplace_back(std::make_shared<BlueprintNode>(GetNextID(), "BP Example", float4(1.0f, 0.5f, 0.5f, 1.0f)));
		blueprintNode->Inputs.emplace_back(GetNextID(), "InFlow", PinType::Flow);
		blueprintNode->Inputs.emplace_back(GetNextID(), "Input", PinType::Float);
		blueprintNode->Outputs.emplace_back(GetNextID(), "OutFlow", PinType::Flow);
		blueprintNode->Outputs.emplace_back(GetNextID(), "Output", PinType::Float);

		auto& bp2Node = m_Nodes.emplace_back(std::make_shared<BlueprintNode>(GetNextID(), "BP Example 2", float4(0.0f, 1.5f, 0.5f, 1.0f)));
		bp2Node->Inputs.emplace_back(GetNextID(), "InFlow", PinType::Flow);
		bp2Node->Inputs.emplace_back(GetNextID(), "Input", PinType::Float);
		bp2Node->Outputs.emplace_back(GetNextID(), "OutFlow", PinType::Flow);
		bp2Node->Outputs.emplace_back(GetNextID(), "Output", PinType::Float);

		auto& stringNode = m_Nodes.emplace_back(std::make_shared<SimpleNode>(GetNextID(), ""));
		stringNode->Outputs.emplace_back(GetNextID(), "Message", PinType::String);

		auto& treeNode = m_Nodes.emplace_back(std::make_shared<TreeNode>(GetNextID(), "Tree Example"));
		treeNode->Inputs.emplace_back(GetNextID(), "InFlow", PinType::Flow);
		treeNode->Outputs.emplace_back(GetNextID(), "OutFlow", PinType::Flow);

		auto& groupNode = m_Nodes.emplace_back(std::make_shared<GroupNode>(GetNextID(), "Group Example"));

		// Build nodes
		BuildNodes();
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
		// Set this blueprint in the node editor
		ImguiExt::SetCurrentEditor(m_Context);
		ImguiExt::Begin(m_Name.c_str());

		// Draw the blueprint
		m_Builder.DrawBlueprint(this);

		if (!m_CreatingNewNode)
		{
			// Check if we should create new nodes or links
			if (ImguiExt::BeginCreate())
			{
				CheckNewNodes();
				CheckNewLinks();
			}

			ImguiExt::EndCreate();

			// Check if we should delete any nodes or links
			if (ImguiExt::BeginDelete())
				CheckDeletions();

			ImguiExt::EndDelete();
		}

		ImguiExt::Suspend();

		// Check the state of context menu popups
		CheckContextMenus();

		ImguiExt::Resume();

		// End the node editor
		ImguiExt::End();
		ImguiExt::SetCurrentEditor(nullptr);
	}

	void Blueprint::InitNodeEditor()
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

		// Create the editor and set it as the current
		m_Context = ImguiExt::CreateEditor(&config);
		ImguiExt::SetCurrentEditor(m_Context);

		// Navigate to the node editor
		ImguiExt::NavigateToContent();
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

	void Blueprint::CheckNewLinks()
	{
		ImguiExt::PinId start, end;

		if (ImguiExt::QueryNewLink(&start, &end))
		{
			Pin* startPin = FindPin(start.Get());
			Pin* endPin = FindPin(end.Get());

			if (startPin->IO == PinIO::Input)
			{
				std::swap(startPin, endPin);
				std::swap(start, end);
			}

			if (startPin && endPin)
			{
				if (startPin == endPin)
				{
					// Reject connecting a pin to itself
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (startPin->Node == endPin->Node)
				{
					// Reject connecting pins within the same node
					m_Builder.DrawLabel("x Incompatible Node", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (startPin->IO == endPin->IO)
				{
					// Reject connecting pins of different types
					m_Builder.DrawLabel("x Incompatible Pin IO", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (startPin->Type != endPin->Type)
				{
					// Reject connecting pins of different types
					m_Builder.DrawLabel("x Incompatible Pin Type", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else
				{
					if (ImguiExt::AcceptNewItem())
					{
						startPin->Linked = true;
						endPin->Linked = true;

						Link& newLink = m_Links.emplace_back(Link(GetNextID(), start.Get(), end.Get()));
						newLink.Color = startPin->GetColor();
					}
				}
			}
		}
	}

	void Blueprint::CheckNewNodes()
	{
		ImguiExt::PinId pinID;

		if (ImguiExt::QueryNewNode(&pinID))
		{
			if (Pin* newLinkPin = FindPin(pinID.Get()))
				m_Builder.DrawLabel("+ Create Node", New_Node_Text_Color);

			if (ImguiExt::AcceptNewItem())
			{
				ImguiExt::Suspend();
				m_CreatingNewNode = true;
				ImGui::OpenPopup("Create New Node");
				ImguiExt::Resume();
			}
		}
		else
		{
			m_CreatingNewNode = false;
		}
	}

	void Blueprint::CheckDeletions()
	{
		ImguiExt::NodeId nodeID = 0;
		ImguiExt::LinkId linkID = 0;

		// Search for deleted nodes
		while (ImguiExt::QueryDeletedNode(&nodeID))
		{
			if (ImguiExt::AcceptDeletedItem())
			{
				auto foundID = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeID](auto& node) { return node->ID == nodeID.Get(); });
				if (foundID != m_Nodes.end())
					m_Nodes.erase(foundID);
			}
		}

		// Search for deleted links
		while (ImguiExt::QueryDeletedLink(&linkID))
		{
			if (ImguiExt::AcceptDeletedItem())
			{
				auto foundID = std::find_if(m_Links.begin(), m_Links.end(), [linkID](Link& link) { return link.ID == linkID.Get(); });
				if (foundID != m_Links.end())
				{
					if (Pin* startPin = FindPin((*foundID).StartPinID))
						startPin->Linked = false;

					if (Pin* endPin = FindPin((*foundID).EndPinID))
						endPin->Linked = false;

					m_Links.erase(foundID);
				}
			}
		}
	}

	void Blueprint::CheckContextMenus()
	{
		static ImguiExt::NodeId contextNode;
		static ImguiExt::PinId contextPin;
		static ImguiExt::LinkId contextLink;

		if (ImguiExt::ShowNodeContextMenu(&contextNode))
			ImGui::OpenPopup("Node Context Menu");
		else if (ImguiExt::ShowPinContextMenu(&contextPin))
			ImGui::OpenPopup("Pin Context Menu");
		else if (ImguiExt::ShowLinkContextMenu(&contextLink))
			ImGui::OpenPopup("Link Context Menu");
		else if (ImguiExt::ShowBackgroundContextMenu())
			ImGui::OpenPopup("Create New Node");

		if (ImGui::BeginPopup("Node Context Menu"))
		{
			Node* node = FindNode(contextNode.Get());

			ImGui::TextUnformatted("Node Context Menu");
			ImGui::Separator();

			if (node)
			{
				ImGui::Text("ID: %d3", node->ID);
				ImGui::Text("Name: %s", node->Name);
				ImGui::Text("Inputs: %d3", (int)node->Inputs.size());
				ImGui::Text("Outputs: %d3", (int)node->Outputs.size());
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
				ed::DeleteNode(contextNode);
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("Pin Context Menu"))
		{
			Pin* pin = FindPin(contextPin.Get());

			ImGui::TextUnformatted("Pin Context Menu");
			ImGui::Separator();
			if (pin)
			{
				ImGui::Text("ID: %d3", pin->ID);
				if (pin->Node)
					ImGui::Text("Node: %d3", pin->Node->ID);
				else
					ImGui::Text("Node: %s", "<none>");
			}
			else
				ImGui::Text("Unknown pin: %d3", contextPin);

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("Link Context Menu"))
		{
			Link* link = FindLink(contextLink.Get());

			ImGui::TextUnformatted("Link Context Menu");
			ImGui::Separator();
			if (link)
			{
				ImGui::Text("ID: %d3", link->ID);
				ImGui::Text("From: %d3", link->StartPinID);
				ImGui::Text("To: %d3", link->EndPinID);
			}
			else
				ImGui::Text("Unknown link: %d3", contextLink);

			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
				ed::DeleteLink(contextLink);

			ImGui::EndPopup();
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