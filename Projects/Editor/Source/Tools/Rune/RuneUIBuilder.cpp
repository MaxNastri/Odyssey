#include "RuneUIBuilder.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
namespace ImguiExt = ax::NodeEditor;

namespace Odyssey::Rune
{
	RuneUIBuilder::RuneUIBuilder(uint64_t texture, int32_t textureWidth, uint64_t textureHeight)
		: HeaderTextureId(texture), HeaderTextureWidth(textureWidth), HeaderTextureHeight(textureHeight),
		CurrentNodeId(0), CurrentStage(Stage::Invalid), HasHeader(false)
	{

	}

	void RuneUIBuilder::Begin(NodeId id)
	{
		HasHeader = false;
		HeaderMin = HeaderMax = float2(0.0f);

		ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

		ImguiExt::BeginNode(id);

		ImGui::PushID(id);
		CurrentNodeId = id;

		SetStage(Stage::Begin);
	}

    static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;

        float2 cursorPos = float2(window->DC.CursorPos.x, window->DC.CursorPos.y);
        ImVec2 itemSize = CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);;
        float2 min = cursorPos + (split_vertically ? float2(*size1, 0.0f) : float2(0.0f, *size1));
        float2 max = min + float2(itemSize.x, itemSize.y);
        bb.Min = ImVec2(min.x, min.y);
        bb.Max = ImVec2(max.x, max.y);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }

    static void ShowLeftPane(float paneWidth)
    {
        auto& io = ImGui::GetIO();

        ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

        paneWidth = ImGui::GetContentRegionAvail().x;

        static bool showStyleEditor = false;
        ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("Zoom to Content"))
            ImguiExt::NavigateToContent();
        ImGui::Spring(0.0f);
        //if (ImGui::Button("Show Flow"))
        //{
        //    for (auto& link : m_Links)
        //        ed::Flow(link.ID);
        //}
        ImGui::Spring();
        if (ImGui::Button("Edit Style"))
            showStyleEditor = true;
        ImGui::EndHorizontal();

        ImGui::EndChild();
    }

    void RuneUIBuilder::DrawNode(Node* node)
    {
        static float leftPaneWidth = 400.0f;
        static float rightPaneWidth = 800.0f;
        Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);
        ShowLeftPane(leftPaneWidth - 4.0f);
        DrawSimpleNode(node);
    }

	void RuneUIBuilder::End()
	{
        SetStage(Stage::End);

        ImguiExt::EndNode();

        if (ImGui::IsItemVisible())
        {
            const float halfBorderWidth = ImguiExt::GetStyle().NodeBorderWidth * 0.5f;
            int32_t alpha = (int32_t)(255 * ImGui::GetStyle().Alpha);
            ImColor headerColor = ImColor(HeaderColor.r, HeaderColor.g, HeaderColor.b, HeaderColor.a);

            ImDrawList* drawList = ImguiExt::GetNodeBackgroundDrawList(CurrentNodeId);

            if ((HeaderMax.x > HeaderMin.x) && (HeaderMax.y > HeaderMin.y) && HeaderTextureId)
            {
                const auto uv = ImVec2(
                    (HeaderMax.x - HeaderMin.x) / (float)(4.0f * HeaderTextureWidth),
                    (HeaderMax.y - HeaderMin.y) / (float)(4.0f * HeaderTextureHeight));

                float2 fMin = HeaderMin - float2(8 - halfBorderWidth, 4 - halfBorderWidth);
                float2 fMax = HeaderMax + float2(8 - halfBorderWidth, 0);
                ImVec2 min = ImVec2(fMin.x, fMin.y);
                ImVec2 max = ImVec2(fMax.x, fMax.y);
                ImVec2 uvMin = ImVec2(0.0f, 0.0f);
#if IMGUI_VERSION_NUM > 18101
                ImDrawFlags drawFlags = ImDrawFlags_RoundCornersTop;
#else
                ImDrawFlags drawFlags = 1 | 2;
#endif
                drawList->AddImageRounded((ImTextureID)HeaderTextureId,
                    min, max,
                    uvMin, uv,
                    headerColor, ImguiExt::GetStyle().NodeRounding, drawFlags);

                if (ContentMin.y > HeaderMax.y)
                {
                    drawList->AddLine(
                        ImVec2(HeaderMin.x - (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                        ImVec2(HeaderMax.x + (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                        ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
                }
            }
        }

        CurrentNodeId = 0;

        ImGui::PopID();

        ImguiExt::PopStyleVar();

        SetStage(Stage::Invalid);
	}

    void RuneUIBuilder::Header(float4 color)
    {
        HeaderColor = color;
        SetStage(Stage::Header);
    }

    void RuneUIBuilder::EndHeader()
    {
        SetStage(Stage::Content);
    }

    void RuneUIBuilder::Input(PinId id)
    {
        if (CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (CurrentStage == Stage::Input);

        SetStage(Stage::Input);

        if (applyPadding)
            ImGui::Spring(0);

        Pin(id, PinIO::Input);

        ImGui::BeginHorizontal(id);
    }

    void RuneUIBuilder::EndInput()
    {
        ImGui::EndHorizontal();
        EndPin();
    }

    void RuneUIBuilder::DrawSimpleNode(Node* node)
    {
        const bool isSimple = node->Type == NodeType::Simple;
        const bool isBlueprint = node->Type == NodeType::Blueprint;

        bool hasOutputDelegates = false;

        for (auto& output : node->Outputs)
        {
            if (output.Type == PinType::Delegate)
            {
                hasOutputDelegates = true;
                break;
            }
        }

        Begin(node->ID);

        // Only blueprints draw a header
        if (isBlueprint)
        {
            Header(node->Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node->Name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            if (hasOutputDelegates)
            {
                ImGui::BeginVertical("delegates", ImVec2(0, 28));
                ImGui::Spring(1, 0);
                for (auto& output : node->Outputs)
                {
                    if (output.Type != PinType::Delegate)
                        continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    //if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                    //    alpha = alpha * (48.0f / 255.0f);

                    ImguiExt::BeginPin(output.ID, ImguiExt::PinKind::Output);
                    ImguiExt::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                    ImguiExt::PinPivotSize(ImVec2(0, 0));
                    ImGui::BeginHorizontal(output.ID);
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
            EndHeader();
        }


        for (auto& input : node->Inputs)
        {
            auto alpha = ImGui::GetStyle().Alpha;
            //if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
            //    alpha = alpha * (48.0f / 255.0f);

            Input(input.ID);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            //DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
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
            EndInput();
        }

        End();
    }

    bool RuneUIBuilder::SetStage(Stage stage)
    {
        if (stage == CurrentStage)
            return false;

        auto oldStage = CurrentStage;
        CurrentStage = stage;

        ImVec2 cursor;
        switch (oldStage)
        {
            case Stage::Begin:
                break;

            case Stage::Header:
                ImGui::EndHorizontal();
                HeaderMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                HeaderMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

                // spacing between header and content
                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

                break;

            case Stage::Content:
                break;

            case Stage::Input:
                ImguiExt::PopStyleVar(2);

                ImGui::Spring(1, 0);
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::Middle:
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::Output:
                ImguiExt::PopStyleVar(2);

                ImGui::Spring(1, 0);
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::End:
                break;

            case Stage::Invalid:
                break;
        }

        switch (stage)
        {
            case Stage::Begin:
                ImGui::BeginVertical("node");
                break;

            case Stage::Header:
                HasHeader = true;

                ImGui::BeginHorizontal("header");
                break;

            case Stage::Content:
                if (oldStage == Stage::Begin)
                    ImGui::Spring(0);

                ImGui::BeginHorizontal("content");
                ImGui::Spring(0, 0);
                break;

            case Stage::Input:
                ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

                if (!HasHeader)
                    ImGui::Spring(1, 0);
                break;

            case Stage::Middle:
                ImGui::Spring(1);
                ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
                break;

            case Stage::Output:
                if (oldStage == Stage::Middle || oldStage == Stage::Input)
                    ImGui::Spring(1);
                else
                    ImGui::Spring(1, 0);
                ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

                if (!HasHeader)
                    ImGui::Spring(1, 0);
                break;

            case Stage::End:
                if (oldStage == Stage::Input)
                    ImGui::Spring(1, 0);
                if (oldStage != Stage::Begin)
                    ImGui::EndHorizontal();
                ContentMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                ContentMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

                //ImGui::Spring(0);
                ImGui::EndVertical();
                NodeMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                NodeMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
                break;

            case Stage::Invalid:
                break;
        }

        return true;
    }

    void RuneUIBuilder::Pin(PinId id, PinIO pinIO)
    {
        ImguiExt::BeginPin(id, pinIO == PinIO::Input ? ImguiExt::PinKind::Input : ImguiExt::PinKind::Output);
    }

    void RuneUIBuilder::EndPin()
    {
        ImguiExt::EndPin();

        // #debug
        // ImGui::GetWindowDrawList()->AddRectFilled(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
    }
}