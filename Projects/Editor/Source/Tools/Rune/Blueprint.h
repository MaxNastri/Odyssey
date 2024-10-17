#pragma once
#include "RuneNodes.h"
#include "Link.h"
#include "RuneUIBuilder.h"

namespace ax::NodeEditor
{
	struct EditorContext;
}
namespace ImguiExt = ax::NodeEditor;

namespace Odyssey::Rune
{
	class Blueprint
	{
	public:
		Blueprint();
		~Blueprint();

	public:
		void Update();

	public:
		std::vector<std::shared_ptr<Node>>& GetNodes() { return m_Nodes; }
		std::vector<Link>& GetLinks() { return m_Links; }
		std::string_view GetName() { return m_Name; }

	private:
		void InitNodeEditor();
		void BuildNodes();
		void BuildNode(Node* node);

	private:
		void CheckNewLinks();
		void CheckNewNodes();
		void CheckDeletions();
		void CheckContextMenus(float2 mousePos);

	private:
		Node* FindNode(NodeId nodeID);
		Link* FindLink(LinkId linkID);
		Pin* FindPin(PinId pinID);

	private:
		void BreakLinks(Pin* pin);

	private:
		size_t LoadNodeSettings(NodeId nodeId, char* data);
		bool SaveNodeSettings(NodeId nodeId, const char* data, size_t size);

	private:
		RuneUIBuilder m_Builder;
		ImguiExt::EditorContext* m_Context = nullptr;
		std::string m_Name;
		std::vector<std::shared_ptr<Node>> m_Nodes;
		std::vector<Link> m_Links;
		bool m_CreatingNewNode = false;
		Pin* m_NewNodeLinkPin = nullptr;
		Pin* m_NewLinkPin = nullptr;

	private:
		inline static constexpr float4 Reject_Link_Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
		inline static constexpr float4 Incompatible_Link_Color = float4(0.17f, 0.12f, 0.12f, 0.70f);
		inline static constexpr float4 New_Node_Text_Color = float4(0.12f, 0.17f, 0.12f, 0.70f);
	};
}