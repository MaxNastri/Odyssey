#include "RuneNodes.h"
#include "RuneInternal.h"
#include "BlueprintBuilder.h"
#include "imgui.hpp"

namespace Odyssey::Rune
{
	Node::Node(std::string_view name, float4 color)
		: Guid(GUID::New()), Name(name), Color(color), Size(0.0f)
	{

	}

	Node::Node(GUID guid, std::string_view name, float4 color)
		: Guid(guid), Name(name), Color(color), Size(0.0f)
	{
	}

	void Node::SetPosition(float2 position)
	{
		if (position != float2(0.0f))
			ImguiExt::SetNodePosition((uint64_t)Guid, position);
	}

	BlueprintNode::BlueprintNode(std::string_view name, float4 color)
		: Node(name, color)
	{
		Inputs.emplace_back("Input", PinType::Flow);
		Outputs.emplace_back("Output", PinType::Flow);
	}

	void BlueprintNode::Draw(BlueprintBuilder* builder, Pin* activeLinkPin)
	{
		builder->BeginNode(Guid);

		bool hasOutputDelegates = false;

		for (auto& output : Outputs)
		{
			if (output.Type == PinType::Delegate)
			{
				hasOutputDelegates = true;
				break;
			}
		}

		// Header
		{
			builder->BeginHeader(Color);
			ImGui::Spring(0);
			ImGui::TextUnformatted(Name.c_str());
			ImGui::Spring(1);
			ImGui::Dummy(ImVec2(0, 28));
			if (hasOutputDelegates)
			{
				ImGui::BeginVertical("delegates", ImVec2(0, 28));
				ImGui::Spring(1, 0);
				for (auto& output : Outputs)
				{
					if (output.Type != PinType::Delegate)
						continue;

					float alpha = ImGui::GetStyle().Alpha;

					if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &output) && &output != activeLinkPin)
						alpha = alpha * (48.0f / 255.0f);

					ImguiExt::BeginPin(output.ID, ImguiExt::PinKind::Output);
					ImguiExt::PinPivotAlignment(ImVec2(1.0f, 0.5f));
					ImguiExt::PinPivotSize(ImVec2(0, 0));
					ImGui::BeginHorizontal((int32_t)output.ID);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
					if (!output.Name.empty())
					{
						ImGui::TextUnformatted(output.Name.c_str());
						ImGui::Spring(0);
					}

					output.Draw(alpha);

					ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
					ImGui::EndHorizontal();
					ImGui::PopStyleVar();
					ImguiExt::EndPin();

					//DrawItemRect(ImColor(255, 0, 0));
				}
				ImGui::Spring(1, 0);
				ImGui::EndVertical();
				ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
			}
			else
				ImGui::Spring(0);

			builder->EndHeader();
		}

		// Inputs
		for (auto& input : Inputs)
		{
			float alpha = ImGui::GetStyle().Alpha;

			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &input) && &input != activeLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			builder->BeginInput(input.ID);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			input.Draw(alpha);
			ImGui::Spring(0);

			if (!input.Name.empty())
			{
				ImGui::TextUnformatted(input.Name.c_str());
				ImGui::Spring(0);
			}

			ImGui::PopStyleVar();

			builder->EndInput();
		}

		// Outputs
		for (auto& output : Outputs)
		{
			if (output.Type == PinType::Delegate)
				continue;

			auto alpha = ImGui::GetStyle().Alpha;
			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &output) && &output != activeLinkPin)
				alpha = alpha * (48.0f / 255.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

			builder->BeginOutput(output.ID);

			if (output.Type == PinType::String)
			{
				static char buffer[128] = "Edit Me\nMultiline!";
				static bool wasActive = false;

				ImGui::PushItemWidth(100.0f);
				ImGui::InputText("##edit", buffer, 127);
				ImGui::PopItemWidth();
				if (ImGui::IsItemActive() && !wasActive)
				{
					ImguiExt::EnableShortcuts(false);
					wasActive = true;
				}
				else if (!ImGui::IsItemActive() && wasActive)
				{
					ImguiExt::EnableShortcuts(true);
					wasActive = false;
				}
				ImGui::Spring(0);
			}

			if (!output.Name.empty())
			{
				ImGui::Spring(0);
				ImGui::TextUnformatted(output.Name.c_str());
			}

			ImGui::Spring(0);
			output.Draw(alpha);
			ImGui::PopStyleVar();

			builder->EndOutput();
		}

		builder->EndNode();
	}

	BranchNode::BranchNode(std::string_view name, float4 color)
		: BlueprintNode(name, color)
	{
		Inputs.clear();
		Outputs.clear();

		Inputs.emplace_back(Pin("", PinType::Flow, false));
		Inputs.emplace_back(Pin("Condition", PinType::Bool, false));

		Outputs.emplace_back(Pin("True", PinType::Flow, false));
		Outputs.emplace_back(Pin("False", PinType::Flow, false));
	}

	SimpleNode::SimpleNode(std::string_view name, float4 color)
		: Node(name, color)
	{
		Outputs.emplace_back(Pin("Value", PinType::Bool, true));
	}

	void SimpleNode::Draw(BlueprintBuilder* builder, Pin* activeLinkPin)
	{
		builder->BeginNode(Guid);

		// Inputs
		for (auto& input : Inputs)
		{
			float alpha = ImGui::GetStyle().Alpha;

			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &input) && &input != activeLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			builder->BeginInput(input.ID);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			input.Draw(alpha);
			ImGui::Spring(0);

			if (!input.Name.empty())
			{
				ImGui::TextUnformatted(input.Name.c_str());
				ImGui::Spring(0);
			}

			if (input.Type == PinType::Bool)
			{
				ImGui::Button("Hello");
				ImGui::Spring(0);
			}

			ImGui::PopStyleVar();

			builder->EndInput();
		}

		// Middle
		{
			builder->Middle();

			ImGui::Spring(1, 0);
			ImGui::TextUnformatted(Name.c_str());
			ImGui::Spring(1, 0);
		}

		// Outputs
		for (auto& output : Outputs)
		{
			if (output.Type == PinType::Delegate)
				continue;

			auto alpha = ImGui::GetStyle().Alpha;
			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &output) && &output != activeLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

			builder->BeginOutput(output.ID);

			if (output.Type == PinType::String)
			{
				static char buffer[128] = "Edit Me\nMultiline!";
				static bool wasActive = false;

				ImGui::PushItemWidth(100.0f);
				ImGui::InputText("##edit", buffer, 127);
				ImGui::PopItemWidth();
				if (ImGui::IsItemActive() && !wasActive)
				{
					ImguiExt::EnableShortcuts(false);
					wasActive = true;
				}
				else if (!ImGui::IsItemActive() && wasActive)
				{
					ImguiExt::EnableShortcuts(true);
					wasActive = false;
				}
				ImGui::Spring(0);
			}

			if (!output.Name.empty())
			{
				ImGui::Spring(0);
				ImGui::TextUnformatted(output.Name.c_str());
			}

			ImGui::Spring(0);
			output.Draw(alpha);
			ImGui::PopStyleVar();

			builder->EndOutput();
		}

		builder->EndNode();
	}

	GroupNode::GroupNode(std::string_view name, float4 color)
		: Node(name, color)
	{
		Size = float2(200, 200);
	}

	void GroupNode::Draw(BlueprintBuilder* builder, Pin* activeLinkPin)
	{
		// Init style and colors
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, Group_Alpha);
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));

		// Begin
		ImguiExt::BeginNode(Guid.CRef());

		// Group content
		{
			ImGui::PushID((int)Guid);
			ImGui::BeginVertical("content");
			ImGui::BeginHorizontal("horizontal");
			ImGui::Spring(1);
			ImGui::TextUnformatted(Name.c_str());
			ImGui::Spring(1);
			ImGui::EndHorizontal();
			ImguiExt::Group(Size);
			ImGui::EndVertical();
			ImGui::PopID();
		}

		// End
		ImguiExt::EndNode();

		// Pop style
		ImguiExt::PopStyleColor(2);
		ImGui::PopStyleVar();

		// Group Hint
		if (ImguiExt::BeginGroupHint(Guid.CRef()))
		{
			auto bgAlpha = ImGui::GetStyle().Alpha;


			float2 min = ImguiExt::GetGroupMin();

			ImGui::SetCursorScreenPos(min - float2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
			ImGui::BeginGroup();
			ImGui::TextUnformatted(Name.c_str());
			ImGui::EndGroup();

			auto drawList = ImguiExt::GetHintBackgroundDrawList();

			ImRect hintBounds = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
			hintBounds.Min = float2(hintBounds.Min) - float2(8, 4);
			hintBounds.Max = float2(hintBounds.Max) + float2(8, 4);

			ImColor backgroundColor = ImColor(Color.r, Color.g, Color.b, Color.a * 0.25f * bgAlpha);
			ImColor backgroundOutline = ImColor(Color.r, Color.g, Color.b, Color.a * 0.5f * bgAlpha);

			drawList->AddRectFilled(hintBounds.GetTL(), hintBounds.GetBR(), backgroundColor, 4.0f);
			drawList->AddRect(hintBounds.GetTL(), hintBounds.GetBR(), backgroundOutline, 4.0f);

		}
		ImguiExt::EndGroupHint();
	}

	TreeNode::TreeNode(std::string_view name, float4 color)
		: Node(name, color)
	{
		Inputs.emplace_back("Input", PinType::Flow, false);
		Outputs.emplace_back("Output", PinType::Flow, false);
	}

	TreeNode::TreeNode(GUID guid, std::string_view name, float4 color)
		: Node(guid, name, color)
	{
		Inputs.emplace_back("Input", PinType::Flow, false);
		Outputs.emplace_back("Output", PinType::Flow, false);
	}

	void TreeNode::Draw(BlueprintBuilder* builder, Pin* activeLinkPin)
	{
		PushStyle();

		ImguiExt::BeginNode(Guid.CRef());

		DrawInputs(activeLinkPin);
		DrawContent(activeLinkPin);
		DrawOutputs(activeLinkPin);

		ImguiExt::EndNode();

		PopStyle();

		DrawBackground(activeLinkPin);
	}

	void TreeNode::PushStyle()
	{
		// Init style and colors
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_PinRect, ImColor(60, 180, 255, 150));
		ImguiExt::PushStyleColor(ImguiExt::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodeRounding, Rounding);
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_LinkStrength, 0.0f);
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinBorderWidth, 1.0f);
		ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinRadius, PinRadius);

	}

	void TreeNode::DrawInputs(Pin* activeLinkPin)
	{
		m_InputAlpha = ImGui::GetStyle().Alpha * Alpha_Multiplier;
		ImGui::BeginVertical((int32_t)Guid);
		ImGui::BeginHorizontal("inputs");
		ImGui::Spring(0, Padding * 2);

		if (!Inputs.empty())
		{
			auto& pin = Inputs[0];
			ImGui::Dummy(ImVec2(0, Padding));
			ImGui::Spring(1, 0);

			m_InputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowSize, 10.0f);
			ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowWidth, 10.0f);
			ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);

			ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Input);
			ImguiExt::PinPivotRect(m_InputsRect.GetTL(), m_InputsRect.GetBR());
			ImguiExt::PinRect(m_InputsRect.GetTL(), m_InputsRect.GetBR());
			ImguiExt::EndPin();

			ImguiExt::PopStyleVar(3);

			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &pin) && &pin != activeLinkPin)
				m_InputAlpha = ImGui::GetStyle().Alpha * (48.0f / 255.0f);
		}
		else
		{
			ImGui::Dummy(ImVec2(0, Padding));
		}
	}

	void TreeNode::DrawContent(Pin* activeLinkPin)
	{
		ImGui::Spring(0, Padding * 2);
		ImGui::EndHorizontal();

		ImGui::BeginHorizontal("ContentFrame");
		ImGui::Spring(1, Padding);

		ImGui::BeginVertical("Content", ImVec2(0.0f, 0.0f));
		ImGui::Dummy(ImVec2(160, 0));
		ImGui::Spring(1);
		ImGui::TextUnformatted(Name.c_str());
		ImGui::Spring(1);
		ImGui::EndVertical();
		m_ContentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		ImGui::Spring(1, Padding);
		ImGui::EndHorizontal();
	}

	void TreeNode::DrawOutputs(Pin* activeLinkPin)
	{
		m_OutputAlpha = ImGui::GetStyle().Alpha * Alpha_Multiplier;

		ImGui::BeginHorizontal("outputs");
		ImGui::Spring(0, Padding * 2);

		if (!Outputs.empty())
		{
			auto& pin = Outputs[0];
			ImGui::Dummy(ImVec2(0, Padding));
			ImGui::Spring(1, 0);
			m_OutputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

			ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Output);
			ImguiExt::PinPivotRect(m_OutputsRect.GetTL(), m_OutputsRect.GetBR());
			ImguiExt::PinRect(m_OutputsRect.GetTL(), m_OutputsRect.GetBR());
			ImguiExt::EndPin();

			ImguiExt::PopStyleVar();

			if (activeLinkPin && !Pin::CanCreateLink(activeLinkPin, &pin) && &pin != activeLinkPin)
				m_OutputAlpha = ImGui::GetStyle().Alpha * (48.0f / 255.0f);
		}
		else
		{
			ImGui::Dummy(ImVec2(0, Padding));
		}

		ImGui::Spring(0, Padding * 2);
		ImGui::EndHorizontal();

		ImGui::EndVertical();
	}

	void TreeNode::DrawBackground(Pin* activeLinkPin)
	{
		const ImVec4 pinBackground = ImguiExt::GetStyle().Colors[ImguiExt::StyleColor_NodeBg];

		auto drawList = ImguiExt::GetNodeBackgroundDrawList(Guid.CRef());
		const ImDrawFlags topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
		const ImDrawFlags bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

		{
			float2 inTopLeft = float2(m_InputsRect.GetTL().x, m_InputsRect.GetTL().y);
			float2 outTopLeft = float2(m_OutputsRect.GetTL().x, m_OutputsRect.GetTL().y);
			float2 outBotRight = float2(m_OutputsRect.GetBR().x, m_OutputsRect.GetBR().y);

			ImColor inputPinColor = ImColor(pinBackground.x, pinBackground.y, pinBackground.z, m_InputAlpha);
			ImColor outputPinColor = ImColor(pinBackground.x, pinBackground.y, pinBackground.z, m_OutputAlpha);

			// Inputs
			drawList->AddRectFilled(ImVec2(inTopLeft.x, inTopLeft.y + 1), m_InputsRect.GetBR(), inputPinColor, 4.0f, bottomRoundCornersFlags);
			drawList->AddRect(ImVec2(inTopLeft.x, inTopLeft.y + 1), m_InputsRect.GetBR(), inputPinColor, 4.0f, bottomRoundCornersFlags);

			// Outputs
			drawList->AddRectFilled(m_OutputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1), outputPinColor, 4.0f, topRoundCornersFlags);
			drawList->AddRect(m_OutputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1), outputPinColor, 4.0f, topRoundCornersFlags);

			// Content
			ImColor contentBackground = ImColor(Color.r, Color.g, Color.b, Color.a);
			ImColor contentOutline = ImColor(Color.r * 2.0f, Color.g * 2.0f, Color.b * 2.0f, Color.a * 0.5f);
			drawList->AddRectFilled(m_ContentRect.GetTL(), m_ContentRect.GetBR(), contentBackground, 0.0f);
			drawList->AddRect(m_ContentRect.GetTL(), m_ContentRect.GetBR(), contentOutline, 0.0f);
		}
	}

	void TreeNode::PopStyle()
	{
		ImguiExt::PopStyleVar(7);
		ImguiExt::PopStyleColor(4);
	}
}