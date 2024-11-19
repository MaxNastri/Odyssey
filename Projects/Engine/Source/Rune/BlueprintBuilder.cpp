#include "BlueprintBuilder.h"
#include "Blueprint.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "Texture2D.h"
#include "Enum.hpp"
#include "Utils.h"

namespace Odyssey::Rune
{
	BlueprintBuilder::BlueprintBuilder(Blueprint* blueprint)
	{
		// Store the blueprint we are building
		m_Blueprint = blueprint;

		// Init the node editor
		ImguiExt::Config config;
		config.SettingsFile = "Blueprints.json";
		config.UserPointer = m_Blueprint;

		// Create the editor and set it as the current
		m_Context = ImguiExt::CreateEditor(&config);
		ImguiExt::SetCurrentEditor(m_Context);

		for (auto& node : blueprint->m_Nodes)
		{
			ImguiExt::SetNodePosition((uint64_t)node->Guid, node->GetInitialPosition());
		}

		// Navigate to the node editor
		ImguiExt::NavigateToContent();

		// Init the drawing state
		m_DrawingState.CurrentNodeID = 0;
		m_DrawingState.CurrentStage = Stage::Invalid;

		// Load the header texture
		m_Header.Texture = AssetManager::LoadAsset<Texture2D>(Header_Texture_GUID);
		m_Header.TextureID = Renderer::AddImguiTexture(m_Header.Texture);
	}

	BlueprintBuilder::~BlueprintBuilder()
	{
		if (m_Context)
		{
			ImguiExt::DestroyEditor(m_Context);
			m_Context = nullptr;
		}
	}

	void BlueprintBuilder::SetEditor()
	{
		ImguiExt::SetCurrentEditor(m_Context);
	}

	void BlueprintBuilder::Begin()
	{
		// Set this blueprint in the node editor
		ImguiExt::SetCurrentEditor(m_Context);
		ImguiExt::Begin(m_Blueprint->GetName().data());
	}

	void BlueprintBuilder::DrawBlueprint()
	{
		// Draw the nodes
		auto& nodes = m_Blueprint->GetNodes();
		for (auto& node : nodes)
		{
			node->Draw(this, m_DrawingState.ActiveLinkPin);
		}

		// Draw the links
		auto& links = m_Blueprint->GetLinks();
		for (Link& link : links)
		{
			ImColor color = ImColor(link.Color.r, link.Color.g, link.Color.b, 1.0f);
			ImguiExt::Link(link.Guid.CRef(), link.StartPinGUID.CRef(), link.EndPinGUID.CRef(), color, 2.0f);
		}

		if (!m_DrawingState.CreatingNewNode)
		{
			// Check if we should create new nodes or links
			if (ImguiExt::BeginCreate())
			{
				CheckNewNodes();
				CheckNewLinks();
			}
			else
			{
				m_DrawingState.ActiveLinkPin = nullptr;
			}

			ImguiExt::EndCreate();

			// Check if we should delete any nodes or links
			if (ImguiExt::BeginDelete())
				CheckDeletions();

			ImguiExt::EndDelete();
		}

		m_DrawingState.MousePos = ImGui::GetMousePos();

		ImguiExt::Suspend();

		// Check the state of context menu popups
		CheckContextMenus();

		ImguiExt::Resume();
	}

	void BlueprintBuilder::DrawLabel(const char* label, float4 color)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
		float2 size = ImGui::CalcTextSize(label);

		float2 padding = ImGui::GetStyle().FramePadding;
		float2 spacing = ImGui::GetStyle().ItemSpacing;
		float2 cursorPos = ImGui::GetCursorPos();
		float2 cursorScreenPos = ImGui::GetCursorScreenPos();
		float2 offset = float2(spacing.x, -spacing.y);

		ImGui::SetCursorPos(cursorPos + offset);

		float2 rectMin = cursorScreenPos - padding + float2(spacing.x, 0.0f);
		float2 rectMax = cursorScreenPos + size + float2(spacing.x + padding.x, 0.0f);

		auto drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(rectMin, rectMax, ImColor(color.r, color.g, color.b, color.a), size.y * 0.15f);
		ImGui::TextUnformatted(label);
	}

	void BlueprintBuilder::End()
	{
		// End the node editor
		ImguiExt::End();
		//ImguiExt::SetCurrentEditor(nullptr);
	}

	void BlueprintBuilder::ConnectNewNode(Node* node)
	{
		ImguiExt::SetCurrentEditor(m_Context);

		node->SetInitialPosition(m_DrawingState.MousePos);
		ImguiExt::SetNodePosition((uint64_t)node->Guid, m_DrawingState.MousePos);

		Pin* startPin = m_DrawingState.NewNodeLinkPin;

		if (startPin)
		{
			auto& pins = startPin->IO == PinIO::Input ? node->Outputs : node->Inputs;

			for (Pin& pin : pins)
			{
				if (Pin::CanCreateLink(startPin, &pin))
				{
					Pin* endPin = &pin;

					if (startPin->IO == PinIO::Input)
						std::swap(startPin, endPin);

					// Cache start/end and spawn add link menu override, if exists
					if (m_UIOverrides.CreateLinkMenuSet)
					{
						m_DrawingState.PendingStartPin = startPin;
						m_DrawingState.PendingEndPin = endPin;

						ImGui::PushOverrideID(m_UIOverrides.CreateLinkMenuID);
						ImGui::OpenPopup(m_UIOverrides.CreateLinkMenu.c_str());
						ImGui::PopID();
					}
					else
					{
						m_Blueprint->AddLink(startPin, endPin);
					}
				}
			}
		}

		ImguiExt::SetCurrentEditor(nullptr);
	}

	bool BlueprintBuilder::GetPendingLinkNodes(GUID& startNode, GUID& endNode)
	{
		if (m_DrawingState.PendingStartPin && m_DrawingState.PendingStartPin->Node &&
			m_DrawingState.PendingEndPin && m_DrawingState.PendingEndPin->Node)
		{
			startNode = m_DrawingState.PendingStartPin->Node->Guid;
			endNode = m_DrawingState.PendingEndPin->Node->Guid;
			return true;
		}

		return false;
	}

	void BlueprintBuilder::ConfirmPendingLink()
	{
		m_Blueprint->AddLink(m_DrawingState.PendingStartPin, m_DrawingState.PendingEndPin);
		ClearPendingLink();
	}

	void BlueprintBuilder::ClearPendingLink()
	{
		m_DrawingState.PendingStartPin = m_DrawingState.PendingEndPin = nullptr;
	}

	void BlueprintBuilder::NavigateToContent(bool zoomIn)
	{
		ImguiExt::NavigateToContent(zoomIn);
	}

	void BlueprintBuilder::BeginNode(GUID guid)
	{
		m_DrawingState.HasHeader = false;
		m_Header.Min = m_Header.Max = float2(0.0f);

		ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

		ImguiExt::BeginNode(guid.CRef());

		ImGui::PushID((int32_t)guid.CRef());
		m_DrawingState.CurrentNodeID = guid;

		SetStage(Stage::Begin);
	}

	void BlueprintBuilder::BeginHeader(float4 color)
	{
		m_Header.Color = color;
		SetStage(Stage::Header);
	}

	void BlueprintBuilder::BeginInput(GUID guid)
	{
		if (m_DrawingState.CurrentStage == Stage::Begin)
			SetStage(Stage::Content);

		const auto applyPadding = (m_DrawingState.CurrentStage == Stage::Input);

		SetStage(Stage::Input);

		if (applyPadding)
			ImGui::Spring(0);

		BeginPin(guid, PinIO::Input);

		ImGui::BeginHorizontal((int32_t)guid.CRef());
	}

	void BlueprintBuilder::BeginOutput(GUID guid)
	{
		if (m_DrawingState.CurrentStage == Stage::Begin)
			SetStage(Stage::Content);

		bool applyPadding = m_DrawingState.CurrentStage == Stage::Output;

		SetStage(Stage::Output);

		if (applyPadding)
			ImGui::Spring(0);

		BeginPin(guid, PinIO::Output);

		ImGui::BeginHorizontal((int32_t)guid.CRef());
	}

	void BlueprintBuilder::BeginPin(GUID guid, PinIO pinIO)
	{
		ImguiExt::BeginPin(guid.CRef(), pinIO == PinIO::Input ? ImguiExt::PinKind::Input : ImguiExt::PinKind::Output);
	}

	void BlueprintBuilder::Middle()
	{
		if (m_DrawingState.CurrentStage == Stage::Begin)
			SetStage(Stage::Content);

		SetStage(Stage::Middle);
	}

	void BlueprintBuilder::EndNode()
	{
		SetStage(Stage::End);

		ImguiExt::EndNode();

		if (ImGui::IsItemVisible())
		{
			const float halfBorderWidth = ImguiExt::GetStyle().NodeBorderWidth * 0.5f;
			int32_t alpha = (int32_t)(255 * ImGui::GetStyle().Alpha);
			ImColor headerColor = ImColor(m_Header.Color.r, m_Header.Color.g, m_Header.Color.b, m_Header.Color.a);

			ImDrawList* drawList = ImguiExt::GetNodeBackgroundDrawList(m_DrawingState.CurrentNodeID.CRef());

			if ((m_Header.Max.x > m_Header.Min.x) && (m_Header.Max.y > m_Header.Min.y) && m_Header.Texture)
			{
				const ImVec2 uv = ImVec2(
					(m_Header.Max.x - m_Header.Min.x) / (float)(4.0f * m_Header.Texture->GetWidth()),
					(m_Header.Max.y - m_Header.Min.y) / (float)(4.0f * m_Header.Texture->GetHeight()));

				float2 min = m_Header.Min - float2(8.0f - halfBorderWidth, 4.0f - halfBorderWidth);
				float2 max = m_Header.Max + float2(8.0f - halfBorderWidth, 0.0f);
				ImVec2 uvMin = ImVec2(0.0f, 0.0f);
#if IMGUI_VERSION_NUM > 18101
				ImDrawFlags drawFlags = ImDrawFlags_RoundCornersTop;
#else
				ImDrawFlags drawFlags = 1 | 2;
#endif
				drawList->AddImageRounded((ImTextureID)m_Header.TextureID,
					min, max,
					uvMin, uv,
					headerColor, ImguiExt::GetStyle().NodeRounding, drawFlags);

				if (m_DrawingState.ContentMin.y > m_Header.Max.y)
				{
					drawList->AddLine(
						ImVec2(m_Header.Min.x - (8 - halfBorderWidth), m_Header.Max.y - 0.5f),
						ImVec2(m_Header.Max.x + (8 - halfBorderWidth), m_Header.Max.y - 0.5f),
						ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
				}
			}
		}

		m_DrawingState.CurrentNodeID = 0;

		ImGui::PopID();

		ImguiExt::PopStyleVar();

		SetStage(Stage::Invalid);
	}

	void BlueprintBuilder::EndHeader()
	{
		SetStage(Stage::Content);
	}

	void BlueprintBuilder::EndInput()
	{
		ImGui::EndHorizontal();
		EndPin();
	}

	void BlueprintBuilder::EndOutput()
	{
		ImGui::EndHorizontal();
		EndPin();
	}

	void BlueprintBuilder::EndPin()
	{
		ImguiExt::EndPin();

		// #debug
		// ImGui::GetWindowDrawList()->AddRectFilled(
		//     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
	}

	void BlueprintBuilder::OverrideNodeMenu(std::string_view menuName, uint32_t menuID)
	{
		m_UIOverrides.NodeMenu = menuName;
		m_UIOverrides.NodeMenuID = menuID;
		m_UIOverrides.NodeMenuSet = true;
	}

	void BlueprintBuilder::OverridePinMenu(std::string_view menuName, uint32_t menuID)
	{
		m_UIOverrides.PinMenu = menuName;
		m_UIOverrides.PinMenuID = menuID;
		m_UIOverrides.PinMenuSet = true;
	}

	void BlueprintBuilder::OverrideLinkMenu(std::string_view menuName, uint32_t menuID)
	{
		m_UIOverrides.LinkMenu = menuName;
		m_UIOverrides.LinkMenuID = menuID;
		m_UIOverrides.LinkMenuSet = true;
	}

	void BlueprintBuilder::OverrideCreateNodeMenu(std::string_view menuName, uint32_t menuID)
	{
		m_UIOverrides.CreateNodeMenu = menuName;
		m_UIOverrides.CreateNodeMenuID = menuID;
		m_UIOverrides.CreateNodeMenuSet = true;
	}

	void BlueprintBuilder::OverrideCreateLinkMenu(std::string_view menuName, uint32_t menuID)
	{
		m_UIOverrides.CreateLinkMenu = menuName;
		m_UIOverrides.CreateLinkMenuID = menuID;
		m_UIOverrides.CreateLinkMenuSet = true;
	}

	void BlueprintBuilder::CheckNewLinks()
	{
		ImguiExt::PinId start, end;

		if (ImguiExt::QueryNewLink(&start, &end))
		{
			Pin* startPin = m_Blueprint->FindPin(start.Get());
			Pin* endPin = m_Blueprint->FindPin(end.Get());

			m_DrawingState.ActiveLinkPin = startPin ? startPin : endPin;

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
					DrawLabel("x Incompatible Node", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (startPin->IO == endPin->IO)
				{
					// Reject connecting pins of different types
					DrawLabel("x Incompatible Pin IO", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (startPin->Type != endPin->Type)
				{
					// Reject connecting pins of different types
					DrawLabel("x Incompatible Pin Type", Incompatible_Link_Color);
					ImguiExt::RejectNewItem(Reject_Link_Color, 2.0f);
				}
				else if (ImguiExt::AcceptNewItem())
				{
					// Cache start/end and spawn add link menu override, if exists
					if (m_UIOverrides.CreateLinkMenuSet)
					{
						m_DrawingState.PendingStartPin = startPin;
						m_DrawingState.PendingEndPin = endPin;

						ImGui::PushOverrideID(m_UIOverrides.CreateLinkMenuID);
						ImGui::OpenPopup(m_UIOverrides.CreateLinkMenu.c_str());
						ImGui::PopID();
					}
					else
					{
						m_Blueprint->AddLink(startPin, endPin);
					}

					m_DrawingState.ActiveLinkPin = nullptr;
				}
			}
		}
	}

	void BlueprintBuilder::CheckNewNodes()
	{
		ImguiExt::PinId pinID;

		if (ImguiExt::QueryNewNode(&pinID))
		{
			if (m_DrawingState.ActiveLinkPin = m_Blueprint->FindPin(pinID.Get()))
				DrawLabel("+ Create Node", New_Node_Text_Color);

			if (ImguiExt::AcceptNewItem())
			{
				m_DrawingState.CreatingNewNode = true;
				m_DrawingState.NewNodeLinkPin = m_Blueprint->FindPin(pinID.Get());
				m_DrawingState.ActiveLinkPin = nullptr;

				ImguiExt::Suspend();

				ImGui::PushOverrideID(m_UIOverrides.CreateNodeMenuID);
				ImGui::OpenPopup(m_UIOverrides.CreateNodeMenu.c_str());
				ImGui::PopID();

				ImguiExt::Resume();
			}
		}
		else
		{
			m_DrawingState.CreatingNewNode = false;
		}
	}

	void BlueprintBuilder::CheckDeletions()
	{
		ImguiExt::NodeId nodeID = 0;
		ImguiExt::LinkId linkID = 0;

		// Search for deleted nodes
		while (ImguiExt::QueryDeletedNode(&nodeID))
		{
			if (ImguiExt::AcceptDeletedItem())
				m_Blueprint->DeleteNode(nodeID.Get());
		}

		// Search for deleted links
		while (ImguiExt::QueryDeletedLink(&linkID))
		{
			if (ImguiExt::AcceptDeletedItem())
				m_Blueprint->DeleteLink(linkID.Get());
		}
	}

	void BlueprintBuilder::CheckContextMenus()
	{
		static ImguiExt::NodeId contextNode;
		static ImguiExt::PinId contextPin;
		static ImguiExt::LinkId contextLink;

		// Check if we need to display a context menu
		if (ImguiExt::ShowNodeContextMenu(&contextNode))
		{
			// Push the ID override for custom menus
			if (m_UIOverrides.NodeMenuSet)
				ImGui::PushOverrideID(m_UIOverrides.NodeMenuID);

			ImGui::OpenPopup(m_UIOverrides.NodeMenu.c_str());

			// Pop the ID override for custom menus
			if (m_UIOverrides.NodeMenuSet)
				ImGui::PopID();
		}
		else if (ImguiExt::ShowPinContextMenu(&contextPin))
		{
			// Push the ID override for custom menus
			if (m_UIOverrides.PinMenuSet)
				ImGui::PushOverrideID(m_UIOverrides.PinMenuID);

			ImGui::OpenPopup(m_UIOverrides.PinMenu.c_str());

			// Pop the ID override for custom menus
			if (m_UIOverrides.PinMenuSet)
				ImGui::PopID();
		}
		else if (ImguiExt::ShowLinkContextMenu(&contextLink))
		{
			// Push the ID override for custom menus
			if (m_UIOverrides.LinkMenuSet)
				ImGui::PushOverrideID(m_UIOverrides.LinkMenuID);

			ImGui::OpenPopup(m_UIOverrides.LinkMenu.c_str());

			// Pop the ID override for custom menus
			if (m_UIOverrides.LinkMenuSet)
				ImGui::PopID();
		}
		else if (ImguiExt::ShowBackgroundContextMenu())
		{
			// Push the ID override for custom menus
			if (m_UIOverrides.CreateNodeMenuSet)
				ImGui::PushOverrideID(m_UIOverrides.CreateNodeMenuID);

			ImGui::OpenPopup(m_UIOverrides.CreateNodeMenu.c_str());
			m_DrawingState.NewNodeLinkPin = nullptr;

			// Pop the ID override for custom menus
			if (m_UIOverrides.CreateNodeMenuSet)
				ImGui::PopID();
		}

		// Open the node menu if not overriden
		if (!m_UIOverrides.NodeMenuSet && ImGui::BeginPopup(m_UIOverrides.NodeMenu.c_str()))
		{
			Node* node = m_Blueprint->FindNode(contextNode.Get());

			ImGui::TextUnformatted("Node Context Menu");
			ImGui::Separator();

			if (node)
			{
				ImGui::Text("ID: %d3", node->Guid);
				ImGui::Text("Name: %s", node->Name);
				ImGui::Text("Inputs: %d3", (int)node->Inputs.size());
				ImGui::Text("Outputs: %d3", (int)node->Outputs.size());
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
				ImguiExt::DeleteNode(contextNode);
			ImGui::EndPopup();
		}

		// Open the pin menu if not overriden
		if (!m_UIOverrides.PinMenuSet && ImGui::BeginPopup(m_UIOverrides.PinMenu.c_str()))
		{
			Pin* pin = m_Blueprint->FindPin(contextPin.Get());

			ImGui::TextUnformatted("Pin Context Menu");
			ImGui::Separator();
			if (pin)
			{
				if (pin->Modifiable && ImGui::BeginMenu("Modify"))
				{
					auto entries = Enum::GetEntries<PinType>();

					for (auto& [pinType, typeName] : entries)
					{
						if (ImGui::MenuItem(typeName.data()))
						{
							pin->Type = pinType;
							m_Blueprint->BreakLinks(pin);
						}
					}

					ImGui::EndMenu();
				}
			}

			ImGui::EndPopup();
		}

		// Open the link menu if not overriden
		if (!m_UIOverrides.LinkMenuSet && ImGui::BeginPopup(m_UIOverrides.LinkMenu.c_str()))
		{
			Link* link = m_Blueprint->FindLink(contextLink.Get());

			ImGui::TextUnformatted("Link Context Menu");
			ImGui::Separator();

			if (link)
			{
				ImGui::Text("ID: %d3", link->Guid);
				ImGui::Text("From: %d3", link->StartPinGUID);
				ImGui::Text("To: %d3", link->EndPinGUID);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Delete"))
				ImguiExt::DeleteLink(contextLink);

			ImGui::EndPopup();
		}

		// Open the create node menu if not overriden
		if (!m_UIOverrides.CreateNodeMenuSet)
		{
			if (ImGui::BeginPopup(m_UIOverrides.CreateNodeMenu.c_str()))
			{
				ImGui::TextUnformatted("Create New Node");
				ImGui::Separator();

				std::shared_ptr<Node> node;

				if (ImGui::MenuItem("Branch"))
					node = m_Blueprint->AddNode<BranchNode>("Branch");
				if (ImGui::MenuItem("Simple"))
					node = m_Blueprint->AddNode<SimpleNode>("Simple");
				if (ImGui::MenuItem("Group"))
					node = m_Blueprint->AddNode<GroupNode>("Group");
				if (ImGui::MenuItem("Tree"))
					node = m_Blueprint->AddNode<TreeNode>("Tree");

				if (node)
				{
					ConnectNewNode(node.get());
					m_DrawingState.CreatingNewNode = false;
				}

				ImGui::EndPopup();
			}
		}
		// Create node menu is overriden and the popup is not open
		else
		{
			ImGui::PushOverrideID(m_UIOverrides.CreateNodeMenuID);

			if (!ImGui::IsPopupOpen(m_UIOverrides.CreateNodeMenu.c_str()))
				m_DrawingState.CreatingNewNode = false;

			ImGui::PopID();
		}
	}

	bool BlueprintBuilder::SetStage(Stage stage)
	{
		if (stage == m_DrawingState.CurrentStage)
			return false;

		Stage oldStage = m_DrawingState.CurrentStage;
		m_DrawingState.CurrentStage = stage;

		ImVec2 cursor;
		switch (oldStage)
		{
			case Stage::Begin:
				break;

			case Stage::Header:
				ImGui::EndHorizontal();
				m_Header.Min = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
				m_Header.Max = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

				// spacing between header and content
				ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

				break;

			case Stage::Content:
				break;

			case Stage::Input:
				ImguiExt::PopStyleVar(2);

				ImGui::Spring(1, 0);
				ImGui::EndVertical();

				// #debug
				// ImGui::GetWindowDrawList()->AddRect(
				//     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				break;

			case Stage::Middle:
				ImGui::EndVertical();

				// #debug
				// ImGui::GetWindowDrawList()->AddRect(
				//     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				break;

			case Stage::Output:
				ImguiExt::PopStyleVar(2);

				ImGui::Spring(1, 0);
				ImGui::EndVertical();

				// #debug
				// ImGui::GetWindowDrawList()->AddRect(
				//     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

				break;

			case Stage::End:
				break;

			case Stage::Invalid:
				break;
		}

		switch (stage)
		{
			case Stage::Begin:
				ImGui::BeginVertical("node");
				break;

			case Stage::Header:
				m_DrawingState.HasHeader = true;

				ImGui::BeginHorizontal("header");
				break;

			case Stage::Content:
				if (oldStage == Stage::Begin)
					ImGui::Spring(0);

				ImGui::BeginHorizontal("content");
				ImGui::Spring(0, 0);
				break;

			case Stage::Input:
				ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

				ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
				ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

				if (!m_DrawingState.HasHeader)
					ImGui::Spring(1, 0);
				break;

			case Stage::Middle:
				ImGui::Spring(1);
				ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
				break;

			case Stage::Output:
				if (oldStage == Stage::Middle || oldStage == Stage::Input)
					ImGui::Spring(1);
				else
					ImGui::Spring(1, 0);
				ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

				ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
				ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

				if (!m_DrawingState.HasHeader)
					ImGui::Spring(1, 0);
				break;

			case Stage::End:
				if (oldStage == Stage::Input)
					ImGui::Spring(1, 0);
				if (oldStage != Stage::Begin)
					ImGui::EndHorizontal();
				m_DrawingState.ContentMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
				m_DrawingState.ContentMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

				//ImGui::Spring(0);
				ImGui::EndVertical();
				m_DrawingState.NodeMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
				m_DrawingState.NodeMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
				break;

			case Stage::Invalid:
				break;
		}

		return true;
	}
}