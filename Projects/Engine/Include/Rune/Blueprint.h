#pragma once
#include "RuneNodes.h"
#include "Link.h"
#include "BlueprintBuilder.h"

namespace Odyssey::Rune
{
	class Blueprint
	{
	public:
		Blueprint();

	public:
		virtual void Update() = 0;

	public:
		template<typename T, typename... Args>
		std::shared_ptr<Node> AddNode(std::string_view nodeName, Args... args)
		{
			static_assert(std::is_base_of<Node, T>::value, "T is not a dervied class of Node.");

			// Create the node
			std::shared_ptr<Node>& node = m_Nodes.emplace_back(std::make_shared<T>(nodeName, std::forward<Args>(args)...));
			
			// Rebuild the node connections
			BuildNode(node.get());
			BuildNodes();

			return node;
		}

	public:
		void AddLink(Pin* start, Pin* end);
		void DeleteNode(NodeId nodeID);
		void DeleteLink(LinkId linkID);

	public:
		Node* FindNode(NodeId nodeID);
		Link* FindLink(LinkId linkID);
		Pin* FindPin(PinId pinID);

	public:
		std::vector<std::shared_ptr<Node>>& GetNodes() { return m_Nodes; }
		std::vector<Link>& GetLinks() { return m_Links; }
		std::string_view GetName() { return m_Name; }

	protected:
		void BuildNodes();
		void BuildNode(Node* node);

	protected:
		void BreakLinks(Pin* pin);

	protected:
		friend class BlueprintBuilder;
		size_t LoadNodeSettings(NodeId nodeId, char* data);
		bool SaveNodeSettings(NodeId nodeId, const char* data, size_t size);

	protected:
		std::unique_ptr<BlueprintBuilder> m_Builder;
		std::string m_Name;
		std::vector<std::shared_ptr<Node>> m_Nodes;
		std::vector<Link> m_Links;
	};
}