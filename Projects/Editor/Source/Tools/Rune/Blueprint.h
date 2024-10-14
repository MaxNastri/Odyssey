#pragma once
#include "Node.h"
#include "Link.h"

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

	private:
		void BuildNodes();
		void BuildNode(Node* node);
		Node* FindNode(NodeId nodeID);
		Link* FindLink(LinkId linkID);

	private:
		size_t LoadNodeSettings(NodeId nodeId, char* data);
		bool SaveNodeSettings(NodeId nodeId, const char* data, size_t size);

	private:
		ImguiExt::EditorContext* m_Context = nullptr;
		std::vector<Node> m_Nodes;
		std::vector<Link> m_Links;
		int32_t m_NextID = 0;
	};
}