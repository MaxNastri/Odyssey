#pragma once
#include "RuneNodes.h"
#include "GUID.h"

namespace Odyssey
{
	class AnimationState;

	struct AnimationStateNode : public Rune::TreeNode
	{
	public:
		AnimationStateNode(std::string_view name, float4 color = Default_Color);
		AnimationStateNode(std::string_view name, std::shared_ptr<AnimationState> state, float4 color = Default_Color);
		AnimationStateNode(GUID guid, std::string_view name, std::shared_ptr<AnimationState> state, float4 color = Default_Color);

	public:
		void SetAnimationState(std::shared_ptr<AnimationState> state);

	protected:
		virtual void PushStyle() override;
		virtual void DrawContent(Rune::Pin* activeLinkPin) override;
		virtual void DrawOutputs(Rune::Pin* activeLinkPin) override;
		virtual void PopStyle() override;

	private:
		std::shared_ptr<AnimationState> m_AnimationState;

	private:
		inline static constexpr float Progress_Bar_Height = 0.3f;
		inline static constexpr float2 Progress_Bar_Padding = float2(0.0f, 1.0f);
		inline static constexpr float4 Progress_Bar_Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	};
}