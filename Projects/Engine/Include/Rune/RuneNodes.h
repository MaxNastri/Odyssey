#pragma once
#include "Pin.h"
#include "imgui.hpp"
#include "GUID.h"

namespace Odyssey::Rune
{
	class BlueprintBuilder;

	struct Node
	{
	public:
		GUID Guid;
		std::string Name;
		std::vector<Pin> Inputs;
		std::vector<Pin> Outputs;
		float4 Color;
		float2 Size;

		std::string State;
		std::string SavedState;

	public:
		Node(std::string_view name, float4 color = float4(1.0f));
		Node(GUID guid, std::string_view name, float4 color = float4(1.0f));

	public:
		virtual void Draw(BlueprintBuilder* builder, Pin* activeLinkPin) = 0;

	public:
		float2 GetInitialPosition() { return m_InitialPosition; }
		void SetInitialPosition(float2 position);

	private:
		float2 m_InitialPosition;
	};

	struct BlueprintNode : Node
	{
	public:
		BlueprintNode(std::string_view name, float4 color = Default_Color);

	public:
		virtual void Draw(BlueprintBuilder* builder, Pin* activeLinkPin) override;

	protected:
		inline static constexpr float4 Default_Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	};

	struct BranchNode : public BlueprintNode
	{
	public:
		BranchNode(std::string_view name, float4 color = Default_Color);
	};

	struct SimpleNode : Node
	{
	public:
		SimpleNode(std::string_view name, float4 color = Default_Color);

	public:
		virtual void Draw(BlueprintBuilder* builder, Pin* activeLinkPin) override;

	protected:
		inline static constexpr float4 Default_Color = float4(1.0f);
	};

	struct GroupNode : Node
	{
	public:
		GroupNode(std::string_view name, float4 color = Default_Color);

	public:
		virtual void Draw(BlueprintBuilder* builder, Pin* activeLinkPin) override;

	protected:
		inline static constexpr float4 Default_Color = float4(1.0f);
		inline static constexpr float Group_Alpha = 0.75f;
		inline static constexpr float Group_Background_Color = 0.75f;
	};

	struct TreeNode : Node
	{
	public:
		TreeNode(std::string_view name, float4 color = Default_Color);
		TreeNode(GUID guid, std::string_view name, float4 color = Default_Color);

	public:
		virtual void Draw(BlueprintBuilder* builder, Pin* activeLinkPin) override;

	protected:
		virtual void PushStyle();
		virtual void DrawInputs(Pin* activeLinkPin);
		virtual void DrawContent(Pin* activeLinkPin);
		virtual void DrawOutputs(Pin* activeLinkPin);
		virtual void DrawBackground(Pin* activeLinkPin);
		virtual void PopStyle();

	protected:
		ImRect m_InputsRect;
		ImRect m_ContentRect;
		ImRect m_OutputsRect;
		inline static constexpr float Alpha_Multiplier = 0.8f;
		float m_OutputAlpha = 1.0f;
		float m_InputAlpha = 1.0f;

	protected:
		inline static constexpr float4 Default_Color = float4(0.1f, 0.25f, 0.5f, 0.8f);
		inline static constexpr float Rounding = 5.0f;
		inline static constexpr float Padding = 12.0f;
		inline static constexpr float PinRadius = 5.0f;
	};

	inline static void CreateBranchNode(std::shared_ptr<Node>& node)
	{
		node->Inputs.clear();
		node->Outputs.clear();

		node->Inputs.emplace_back(Pin("", PinType::Flow, false));
		node->Inputs.emplace_back(Pin("Condition", PinType::Bool, false));

		node->Outputs.emplace_back(Pin("True", PinType::Flow, false));
		node->Outputs.emplace_back(Pin("False", PinType::Flow, false));
	}
}