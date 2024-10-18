#include "AnimationNodes.h"
#include "OdysseyTime.h"

namespace Odyssey
{
	AnimationStateNode::AnimationStateNode(std::string_view name, float4 color)
		: TreeNode(name, color)
	{

	}

	void AnimationStateNode::PushStyle()
	{
		TreeNode::PushStyle();
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Progress_Bar_Color);
	}

	void AnimationStateNode::DrawContent(Rune::Pin* activeLinkPin)
	{
		m_Progress += Time::DeltaTime();
		m_Progress = std::fmod(m_Progress, 1.0f);

		TreeNode::DrawContent(activeLinkPin);

		float2 size = float2(m_ContentRect.GetWidth(), m_ContentRect.GetHeight() * Progress_Bar_Height);
		ImGui::SetCursorPos(m_ContentRect.GetBL() + Progress_Bar_Padding);
		ImGui::ProgressBar(m_Progress, size, "");
	}
	void AnimationStateNode::PopStyle()
	{
		TreeNode::PopStyle();
		ImGui::PopStyleColor(1);
	}
}