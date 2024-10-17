#pragma once
#include "Pin.h"

namespace Odyssey::Rune
{
	typedef uint64_t NodeId;
	class RuneUIBuilder;

	struct Node
	{
	public:
		NodeId ID;
		std::string Name;
		std::vector<Pin> Inputs;
		std::vector<Pin> Outputs;
		float4 Color;
		float2 Size;

		std::string State;
		std::string SavedState;

	public:
		Node(std::string_view name, float4 color = float4(1.0f));

	public:
		virtual void Draw(RuneUIBuilder* builder) = 0;
	};

	struct BlueprintNode : Node
	{
	public:
		BlueprintNode(std::string_view name, float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f));

	public:
		virtual void Draw(RuneUIBuilder* builder) override;
	};

	struct BranchNode : public BlueprintNode
	{
		BranchNode(std::string_view name);
	};

	struct SimpleNode : Node
	{
	public:
		SimpleNode(std::string_view name, float4 color = float4(1.0f));

	public:
		virtual void Draw(RuneUIBuilder* builder) override;
	};

	struct GroupNode : Node
	{
	public:
		GroupNode(std::string_view name, float4 color = float4(1.0f));

	public:
		virtual void Draw(RuneUIBuilder* builder) override;

	private:
		inline static constexpr float Group_Alpha = 0.75f;
		inline static constexpr float Group_Background_Color = 0.75f;
	};

	struct TreeNode : Node
	{
	public:
		TreeNode(std::string_view name, float4 color = float4(0.1f, 0.25f, 0.5f, 0.8f));

	public:
		virtual void Draw(RuneUIBuilder* builder) override;

	private:
		inline static constexpr float Rounding = 5.0f;
		inline static constexpr float Padding = 12.0f;
		inline static constexpr float PinRadius = 5.0f;
	};
}