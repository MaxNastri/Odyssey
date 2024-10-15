#pragma once
#include "Node.h"
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
		std::vector<Node>& GetNodes() { return m_Nodes; }
		std::vector<Link>& GetLinks() { return m_Links; }
		std::string_view GetName() { return m_Name; }

	private:
		void InitNodeEditor();
		void BuildNodes();
		void BuildNode(Node* node);

	private:
		void UpdateCreation();
		void UpdateDeletion();

	private:
		Node* FindNode(NodeId nodeID);
		Link* FindLink(LinkId linkID);

	private:
		size_t LoadNodeSettings(NodeId nodeId, char* data);
		bool SaveNodeSettings(NodeId nodeId, const char* data, size_t size);

	private:
		RuneUIBuilder m_Builder;
		ImguiExt::EditorContext* m_Context = nullptr;
		std::string m_Name;
		std::vector<Node> m_Nodes;
		std::vector<Link> m_Links;
		uint64_t m_NextID = 0;
	};
}