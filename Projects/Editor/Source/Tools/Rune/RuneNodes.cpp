#include "RuneNodes.h"
#include "RuneUIBuilder.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include "widgets.h"

namespace Odyssey
{
	namespace Rune
	{
		namespace ImguiExt = ax::NodeEditor;

		Node::Node(NodeId id, std::string_view name, float4 color)
			: ID(id), Name(name), Color(color), Size(0.0f)
		{

		}

		void Node::DrawPin(const Pin& pin, bool connected, float alpha)
		{
			
		}

		BlueprintNode::BlueprintNode(NodeId id, std::string_view name, float4 color)
			: Node(id, name, color)
		{

		}

		void BlueprintNode::Draw(RuneUIBuilder* builder)
		{
			builder->BeginNode(ID);

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

						auto alpha = ImGui::GetStyle().Alpha;
						//if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
						//    alpha = alpha * (48.0f / 255.0f);

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
						//DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
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
				//if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
				//    alpha = alpha * (48.0f / 255.0f);

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

			// Outputs
			for (auto& output : Outputs)
			{
				if (output.Type == PinType::Delegate)
					continue;

				auto alpha = ImGui::GetStyle().Alpha;
				//if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
				//    alpha = alpha * (48.0f / 255.0f);
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

		TreeNode::TreeNode(NodeId id, std::string_view name, float4 color)
			: Node(id, name, color)
		{

		}

		void TreeNode::Draw(RuneUIBuilder* builder)
		{
			const ImVec4 pinBackground = ImguiExt::GetStyle().Colors[ImguiExt::StyleColor_NodeBg];

			ImRect inputsRect;
			ImRect contentRect;
			ImRect outputsRect;
			float outputAlpha = 0.8f;
			float inputAlpha = 0.8f;

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

			// Start the node
			ImguiExt::BeginNode(ID);

			// Inputs
			{
				ImGui::BeginVertical((int32_t)ID);
				ImGui::BeginHorizontal("inputs");
				ImGui::Spring(0, Padding * 2);

				if (!Inputs.empty())
				{
					auto& pin = Inputs[0];
					ImGui::Dummy(ImVec2(0, Padding));
					ImGui::Spring(1, 0);

					inputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

					ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowSize, 10.0f);
					ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowWidth, 10.0f);
					ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);

					ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Input);
					ImguiExt::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
					ImguiExt::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
					ImguiExt::EndPin();

					ImguiExt::PopStyleVar(3);

					//if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					//    inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
				}
				else
				{
					ImGui::Dummy(ImVec2(0, Padding));
				}
			}
			
			// Content
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
				contentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

				ImGui::Spring(1, Padding);
				ImGui::EndHorizontal();
			}

			// Outputs
			{

				ImGui::BeginHorizontal("outputs");
				ImGui::Spring(0, Padding * 2);

				if (!Outputs.empty())
				{
					auto& pin = Outputs[0];
					ImGui::Dummy(ImVec2(0, Padding));
					ImGui::Spring(1, 0);
					outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

					ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

					ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Output);
					ImguiExt::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
					ImguiExt::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
					ImguiExt::EndPin();

					ImguiExt::PopStyleVar();

					//if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					//    outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
				}
				else
				{
					ImGui::Dummy(ImVec2(0, Padding));
				}

				ImGui::Spring(0, Padding * 2);
				ImGui::EndHorizontal();

				ImGui::EndVertical();
			}

			ImguiExt::EndNode();
			ImguiExt::PopStyleVar(7);
			ImguiExt::PopStyleColor(4);

			// Drawing
			{
				auto drawList = ImguiExt::GetNodeBackgroundDrawList(ID);
				const ImDrawFlags topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
				const ImDrawFlags bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

				{
					float2 inTopLeft = float2(inputsRect.GetTL().x, inputsRect.GetTL().y);
					float2 outTopLeft = float2(outputsRect.GetTL().x, outputsRect.GetTL().y);
					float2 outBotRight = float2(outputsRect.GetBR().x, outputsRect.GetBR().y);

					ImColor inputPinColor = ImColor(pinBackground.x, pinBackground.y, pinBackground.z, inputAlpha);
					ImColor outputPinColor = ImColor(pinBackground.x, pinBackground.y, pinBackground.z, outputAlpha);

					// Inputs
					drawList->AddRectFilled(ImVec2(inTopLeft.x, inTopLeft.y + 1), inputsRect.GetBR(), inputPinColor, 4.0f, bottomRoundCornersFlags);
					drawList->AddRect(ImVec2(inTopLeft.x, inTopLeft.y + 1), inputsRect.GetBR(), inputPinColor, 4.0f, bottomRoundCornersFlags);

					// Outputs
					drawList->AddRectFilled(outputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1), outputPinColor, 4.0f, topRoundCornersFlags);
					drawList->AddRect(outputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1), outputPinColor, 4.0f, topRoundCornersFlags);

					// Content
					// IM_COL32(24, 64, 128, 200), 0.0f)
					ImColor contentBackground = ImColor(Color.r, Color.g, Color.b, Color.a);
					ImColor contentOutline = ImColor(Color.r * 2.0f, Color.g * 2.0f, Color.b * 2.0f, Color.a * 0.5f);
					drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), contentBackground, 0.0f);
					drawList->AddRect(contentRect.GetTL(), contentRect.GetBR(), contentOutline, 0.0f);
				}
			}
		}
	}
}