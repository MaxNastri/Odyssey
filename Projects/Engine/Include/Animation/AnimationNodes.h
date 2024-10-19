#pragma once
#include "RuneNodes.h"

namespace Odyssey
{
	struct AnimationStateNode : public Rune::TreeNode
	{
	public:
		AnimationStateNode(std::string_view name, float4 color = Default_Color);

	protected:
		virtual void PushStyle() override;
		virtual void DrawContent(Rune::Pin* activeLinkPin) override;
		virtual void DrawOutputs(Rune::Pin* activeLinkPin) override;
		virtual void PopStyle() override;

	private:
		float m_Progress = 0.0f;
		inline static constexpr float Progress_Bar_Height = 0.3f;
		inline static constexpr float2 Progress_Bar_Padding = float2(0.0f, 1.0f);
		inline static constexpr float4 Progress_Bar_Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	};
}