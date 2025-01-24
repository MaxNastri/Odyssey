#ifndef IMGUI_DEFINE_MATH_OPERATORS
# define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>
#include "widgets.h"

namespace ImGui
{
	void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color, const ImVec4& innerColor)
	{
		if (ImGui::IsRectVisible(size))
		{
			auto cursorPos = ImGui::GetCursorScreenPos();
			auto drawList = ImGui::GetWindowDrawList();
			DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
		}

		ImGui::Dummy(size);
	}

	void ImageIcon(const ImVec2& size, ImTextureID image, bool filled, float iconSpacing, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
	{
		if (ImGui::IsRectVisible(size))
		{
			auto cursorPos = ImGui::GetCursorScreenPos();
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 spacing(iconSpacing, iconSpacing);

			cursorPos += ImVec2(spacing - size) / 2.0f;

			drawList->AddImage(image, cursorPos, cursorPos + size, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(color));
			//ax::Drawing::DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
		}

		ImGui::Dummy(ImVec2(iconSpacing, iconSpacing));
	}

	void IconGrid(const ImVec2& size, bool filled, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
	{
		if (ImGui::IsRectVisible(size))
		{
			auto cursorPos = ImGui::GetCursorScreenPos();
			auto drawList = ImGui::GetWindowDrawList();

			auto rect = ImRect(cursorPos, cursorPos + size);
			auto rect_x = rect.Min.x;
			auto rect_y = rect.Min.y;
			auto rect_w = rect.Max.x - rect.Min.x;
			auto rect_h = rect.Max.y - rect.Min.y;
			auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
			auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
			auto rect_center = ImVec2(rect_center_x, rect_center_y);
			const auto outline_scale = rect_w / 24.0f;
			const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

			auto triangleStart = rect_center_x + 0.32f * rect_w;

			{
				const auto r = 0.5f * rect_w / 2.0f;
				const auto w = ceilf(r / 3.0f);

				const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
				const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

				auto tl = baseTl;
				auto br = baseBr;
				for (int i = 0; i < 3; ++i)
				{
					tl.x = baseTl.x;
					br.x = baseBr.x;
					drawList->AddRectFilled(tl, br, ImColor(color));
					tl.x += w * 2;
					br.x += w * 2;
					if (i != 1 || filled)
						drawList->AddRectFilled(tl, br, ImColor(color));
					tl.x += w * 2;
					br.x += w * 2;
					drawList->AddRectFilled(tl, br, ImColor(color));

					tl.y += w * 2;
					br.y += w * 2;
				}

				triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
			}

		}

		ImGui::Dummy(size);
	}

	void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor)
	{
		auto rect = ImRect(a, b);
		auto rect_x = rect.Min.x;
		auto rect_y = rect.Min.y;
		auto rect_w = rect.Max.x - rect.Min.x;
		auto rect_h = rect.Max.y - rect.Min.y;
		auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
		auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
		auto rect_center = ImVec2(rect_center_x, rect_center_y);
		const auto outline_scale = rect_w / 24.0f;
		const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

		if (type == IconType::Flow)
		{
			const auto origin_scale = rect_w / 24.0f;

			const auto offset_x = 1.0f * origin_scale;
			const auto offset_y = 0.0f * origin_scale;
			const auto margin = (filled ? 2.0f : 2.0f) * origin_scale;
			const auto rounding = 0.1f * origin_scale;
			const auto tip_round = 0.7f; // percentage of triangle edge (for tip)
			//const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
			const auto canvas = ImRect(
				rect.Min.x + margin + offset_x,
				rect.Min.y + margin + offset_y,
				rect.Max.x - margin + offset_x,
				rect.Max.y - margin + offset_y);
			const auto canvas_x = canvas.Min.x;
			const auto canvas_y = canvas.Min.y;
			const auto canvas_w = canvas.Max.x - canvas.Min.x;
			const auto canvas_h = canvas.Max.y - canvas.Min.y;

			const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
			const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
			const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
			const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
			const auto center_y = (top + bottom) * 0.5f;
			//const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

			const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
			const auto tip_right = ImVec2(right, center_y);
			const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

			drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
			drawList->PathBezierCubicCurveTo(
				ImVec2(left, top),
				ImVec2(left, top),
				ImVec2(left, top) + ImVec2(rounding, 0));
			drawList->PathLineTo(tip_top);
			drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
			drawList->PathBezierCubicCurveTo(
				tip_right,
				tip_right,
				tip_bottom + (tip_right - tip_bottom) * tip_round);
			drawList->PathLineTo(tip_bottom);
			drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
			drawList->PathBezierCubicCurveTo(
				ImVec2(left, bottom),
				ImVec2(left, bottom),
				ImVec2(left, bottom) - ImVec2(0, rounding));

			if (!filled)
			{
				if (innerColor & 0xFF000000)
					drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

				drawList->PathStroke(color, true, 2.0f * outline_scale);
			}
			else
				drawList->PathFillConvex(color);
		}
		else
		{
			auto triangleStart = rect_center_x + 0.32f * rect_w;

			auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

			rect.Min.x += rect_offset;
			rect.Max.x += rect_offset;
			rect_x += rect_offset;
			rect_center_x += rect_offset * 0.5f;
			rect_center.x += rect_offset * 0.5f;

			if (type == IconType::Circle)
			{
				const auto c = rect_center;

				if (!filled)
				{
					const auto r = 0.5f * rect_w / 2.0f - 0.5f;

					if (innerColor & 0xFF000000)
						drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
					drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
				}
				else
				{
					drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
				}
			}

			if (type == IconType::Square)
			{
				if (filled)
				{
					const auto r = 0.5f * rect_w / 2.0f;
					const auto p0 = rect_center - ImVec2(r, r);
					const auto p1 = rect_center + ImVec2(r, r);

#if IMGUI_VERSION_NUM > 18101
					drawList->AddRectFilled(p0, p1, color, 0, ImDrawFlags_RoundCornersAll);
#else
					drawList->AddRectFilled(p0, p1, color, 0, 15);
#endif
				}
				else
				{
					const auto r = 0.5f * rect_w / 2.0f - 0.5f;
					const auto p0 = rect_center - ImVec2(r, r);
					const auto p1 = rect_center + ImVec2(r, r);

					if (innerColor & 0xFF000000)
					{
#if IMGUI_VERSION_NUM > 18101
						drawList->AddRectFilled(p0, p1, innerColor, 0, ImDrawFlags_RoundCornersAll);
#else
						drawList->AddRectFilled(p0, p1, innerColor, 0, 15);
#endif
					}

#if IMGUI_VERSION_NUM > 18101
					drawList->AddRect(p0, p1, color, 0, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
#else
					drawList->AddRect(p0, p1, color, 0, 15, 2.0f * outline_scale);
#endif
				}
			}

			if (type == IconType::Grid)
			{
				const auto r = 0.5f * rect_w / 2.0f;
				const auto w = ceilf(r / 3.0f);

				const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
				const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

				auto tl = baseTl;
				auto br = baseBr;
				for (int i = 0; i < 3; ++i)
				{
					tl.x = baseTl.x;
					br.x = baseBr.x;
					drawList->AddRectFilled(tl, br, color);
					tl.x += w * 2;
					br.x += w * 2;
					if (i != 1 || filled)
						drawList->AddRectFilled(tl, br, color);
					tl.x += w * 2;
					br.x += w * 2;
					drawList->AddRectFilled(tl, br, color);

					tl.y += w * 2;
					br.y += w * 2;
				}

				triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
			}

			if (type == IconType::RoundSquare)
			{
				if (filled)
				{
					const auto r = 0.5f * rect_w / 2.0f;
					const auto cr = r * 0.5f;
					const auto p0 = rect_center - ImVec2(r, r);
					const auto p1 = rect_center + ImVec2(r, r);

#if IMGUI_VERSION_NUM > 18101
					drawList->AddRectFilled(p0, p1, color, cr, ImDrawFlags_RoundCornersAll);
#else
					drawList->AddRectFilled(p0, p1, color, cr, 15);
#endif
				}
				else
				{
					const auto r = 0.5f * rect_w / 2.0f - 0.5f;
					const auto cr = r * 0.5f;
					const auto p0 = rect_center - ImVec2(r, r);
					const auto p1 = rect_center + ImVec2(r, r);

					if (innerColor & 0xFF000000)
					{
#if IMGUI_VERSION_NUM > 18101
						drawList->AddRectFilled(p0, p1, innerColor, cr, ImDrawFlags_RoundCornersAll);
#else
						drawList->AddRectFilled(p0, p1, innerColor, cr, 15);
#endif
					}

#if IMGUI_VERSION_NUM > 18101
					drawList->AddRect(p0, p1, color, cr, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
#else
					drawList->AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
#endif
				}
			}
			else if (type == IconType::Diamond)
			{
				if (filled)
				{
					const auto r = 0.607f * rect_w / 2.0f;
					const auto c = rect_center;

					drawList->PathLineTo(c + ImVec2(0, -r));
					drawList->PathLineTo(c + ImVec2(r, 0));
					drawList->PathLineTo(c + ImVec2(0, r));
					drawList->PathLineTo(c + ImVec2(-r, 0));
					drawList->PathFillConvex(color);
				}
				else
				{
					const auto r = 0.607f * rect_w / 2.0f - 0.5f;
					const auto c = rect_center;

					drawList->PathLineTo(c + ImVec2(0, -r));
					drawList->PathLineTo(c + ImVec2(r, 0));
					drawList->PathLineTo(c + ImVec2(0, r));
					drawList->PathLineTo(c + ImVec2(-r, 0));

					if (innerColor & 0xFF000000)
						drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

					drawList->PathStroke(color, true, 2.0f * outline_scale);
				}
			}
			else
			{
				const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

				drawList->AddTriangleFilled(
					ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
					ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
					ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
					color);
			}
		}
	}

	bool Splitter(bool verticalSplit, float thickness, float2& size, float2 minSize, float2& minBounds, float2& maxBounds)
	{
		using namespace ImGui;
		const float longAxisSize = -1.0f;

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");

		ImRect bounds;
		bounds.Min = window->DC.CursorPos + (verticalSplit ? ImVec2(size.x, 0.0f) : ImVec2(0.0f, size.x));
		bounds.Max = bounds.Min + CalcItemSize(verticalSplit ? ImVec2(thickness, longAxisSize) : ImVec2(longAxisSize, thickness), 0.0f, 0.0f);

		minBounds = bounds.Min;
		maxBounds = bounds.Max;

		return SplitterBehavior(bounds, id, verticalSplit ? ImGuiAxis_X : ImGuiAxis_Y, &size.x, &size.y, minSize.x, minSize.y, 0.0f);
	}

	bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImVec2 pos = window->DC.CursorPos;

		PushID(str_id);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));

		float minHeight = ImGui::CalcTextSize(buf).y + g.Style.FramePadding.y * 2.0f;
		float2 minSize = float2(0.0f, minHeight);
		bool ret = Selectable("##Selectable", selected, flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap, minSize);
		PopStyleVar();

		ImGuiID id = window->GetID("##Input");
		bool temp_input_is_active = TempInputIsActive(id);
		bool temp_input_start = ret ? IsMouseDoubleClicked(0) : false;
		if (temp_input_is_active || temp_input_start)
		{
			auto rect = g.LastItemData.Rect;
			rect.Min = pos;

			if (!temp_input_is_active && !ItemHoverable(rect, id))
			{
				PopID();
				return ret;
			}

			ret = TempInputText(rect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
			KeepAliveID(id);
		}
		else
		{
			window->DrawList->AddText(pos, GetColorU32(ImGuiCol_Text), buf);
		}

		PopID();
		return ret;
	}

	void FilledRectSpan(float4 color, float height, float2 padding)
	{
		auto& style = ImGui::GetStyle();

		float2 windowPadding = style.WindowPadding;
		float2 windowPos = ImGui::GetWindowPos();
		float2 screenPos = ImGui::GetCursorScreenPos();

		// Get the min and max without window padding
		float2 min = ImGui::GetWindowContentRegionMin() + windowPos - windowPadding + padding;
		float2 max = float2((ImGui::GetWindowContentRegionMax() + windowPos).x + windowPadding.x - padding.x, min.y + height);

		// Draw the rect
		ImGui::GetWindowDrawList()->PushClipRect(min, max);
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImColor(color));
		ImGui::GetWindowDrawList()->PopClipRect();

		// Set the cursor below the rect
		ImGui::SetCursorScreenPos(float2(screenPos.x, max.y));
	}

	void FilledRectSpanText(std::string_view text, float4 textColor, float4 bgColor, float height, float2 padding)
	{
		auto& style = ImGui::GetStyle();

		float2 windowPadding = style.WindowPadding - float2(1.0f);
		float2 windowPos = ImGui::GetWindowPos();
		float2 screenPos = ImGui::GetCursorScreenPos();

		// Get the min and max without window padding
		float2 min = ImGui::GetWindowContentRegionMin() + windowPos - windowPadding + padding;
		float2 max = float2((ImGui::GetWindowContentRegionMax() + windowPos).x + windowPadding.x + padding.x, min.y + height);

		// Draw the rect and text
		ImGui::GetWindowDrawList()->PushClipRect(min, max);
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImColor(bgColor));
		ImGui::GetWindowDrawList()->AddText(float2(screenPos.x, min.y), ImColor(textColor), text.data());
		ImGui::GetWindowDrawList()->PopClipRect();

		// Set the cursor below the rect
		ImGui::SetCursorScreenPos(float2(screenPos.x, max.y + (style.ItemSpacing.y * 2.0f)));
	}

	void FilledRectSpanTextFree(std::string_view text, float4 textColor, float4 bgColor, float height, float2 padding)
	{
		auto& style = ImGui::GetStyle();

		float2 windowPadding = style.WindowPadding - float2(1.0f);
		float2 windowPos = ImGui::GetWindowPos();
		float2 screenPos = ImGui::GetCursorScreenPos();

		// Get the min and max without window padding
		float2 min = screenPos - windowPadding + padding;
		float2 max = float2((ImGui::GetWindowContentRegionMax() + windowPos).x + windowPadding.x + padding.x, min.y + height);

		// Draw the rect and text
		ImGui::GetWindowDrawList()->PushClipRect(min, max);
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImColor(bgColor));
		ImGui::GetWindowDrawList()->AddText(float2(screenPos.x, min.y), ImColor(textColor), text.data());
		ImGui::GetWindowDrawList()->PopClipRect();

		// Set the cursor below the rect
		ImGui::SetCursorScreenPos(float2(screenPos.x, max.y + (style.ItemSpacing.y)));
	}
}