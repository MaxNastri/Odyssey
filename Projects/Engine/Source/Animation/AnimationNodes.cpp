#include "AnimationNodes.h"
#include "imgui.hpp"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "AnimationState.h"

namespace Odyssey
{
	AnimationStateNode::AnimationStateNode(std::string_view name, float4 color)
		: TreeNode(name, color)
	{

	}

	AnimationStateNode::AnimationStateNode(std::string_view name, std::shared_ptr<AnimationState> state, float4 color)
		: TreeNode(name, color)
	{
		SetAnimationState(state);
	}

	void AnimationStateNode::SetAnimationState(std::shared_ptr<AnimationState> state)
	{
		m_AnimationState = state;
	}

	void AnimationStateNode::PushStyle()
	{
		TreeNode::PushStyle();
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Progress_Bar_Color);
	}

	void AnimationStateNode::DrawContent(Rune::Pin* activeLinkPin)
	{
		float progress = 0.0f;
		if (m_AnimationState && m_AnimationState->GetClip())
		{
			auto clip = AssetManager::LoadAsset<AnimationClip>(m_AnimationState->GetClip());
			progress = clip->GetProgress();
		}

		TreeNode::DrawContent(activeLinkPin);

		float2 size = float2(m_ContentRect.GetWidth(), m_ContentRect.GetHeight() * Progress_Bar_Height);
		ImGui::SetCursorPos(m_ContentRect.GetBL() + Progress_Bar_Padding);
		ImGui::ProgressBar(progress, size, "");
	}

	void AnimationStateNode::DrawOutputs(Rune::Pin* activeLinkPin)
	{
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowSize, 10.0f);
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowWidth, 10.0f);

		TreeNode::DrawOutputs(activeLinkPin);

		ImguiExt::PopStyleVar(2);

	}
	void AnimationStateNode::PopStyle()
	{
		TreeNode::PopStyle();
		ImGui::PopStyleColor(1);
	}
}