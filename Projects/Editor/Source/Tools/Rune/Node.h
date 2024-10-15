#pragma once
#include "Pin.h"

namespace Odyssey::Rune
{
	enum class NodeType
	{
		None = 0,
		Blueprint = 1,
		Simple = 2,
		Tree = 3,
		Comment = 4,
	};

	typedef uint64_t NodeId;

	struct Node
	{
	public:
		NodeId ID;
		std::string Name;
		std::vector<Pin> Inputs;
		std::vector<Pin> Outputs;
		float4 Color;
		float2 Size;
		NodeType Type;

		std::string State;
		std::string SavedState;

	public:
		Node(NodeId id, std::string_view name, float4 color = float4(1.0f))
			: ID(id), Name(name), Color(color), Size(0.0f), Type(NodeType::Blueprint)
		{

		}
	};
}