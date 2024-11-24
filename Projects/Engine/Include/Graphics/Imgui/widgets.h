#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
	enum class IconType : ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };
	void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));
	void ImageIcon(const ImVec2& size, ImTextureID image, bool filled, float iconSpacing, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));
	void IconGrid(const ImVec2& size, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));
	void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);
	bool Splitter(bool verticalSplit, float thickness, float2& size, float2 minSize, float2& minBounds, float2& maxBounds);
	bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size);
	void FilledRectSpan(float4 color, float height, float2 padding);
	void FilledRectSpanText(std::string_view text, float4 textColor, float4 bgColor, float height, float2 padding);
	void FilledRectSpanTextFree(std::string_view text, float4 textColor, float4 bgColor, float height, float2 padding);
}