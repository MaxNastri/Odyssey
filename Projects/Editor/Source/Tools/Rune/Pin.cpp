#include "Pin.h"
#include "imgui.h"
#include "widgets.h"

namespace Odyssey::Rune
{
	Pin::Pin(PinId id, std::string_view name, PinType type)
		: ID(id), Name(name), Node(nullptr), IO(PinIO::Input), Type(type)
	{

	}

	void Pin::Draw(float alpha)
	{
		using namespace ImGui::Widgets;

		IconType iconType;
		float4 color = float4(Pin_Colors[Type], alpha);

		switch (Type)
		{
			case PinType::Bool:
			case PinType::Int:
			case PinType::Float:
			case PinType::String:
			case PinType::Object:
			case PinType::Function:
				iconType = IconType::Circle;
				break;
			case PinType::Flow:
				iconType = IconType::Flow;
				break;
			case PinType::Delegate:
				iconType = IconType::Square;
				break;
			default:
				return;
		}

		ImColor pinColor = ImColor(color.r, color.g, color.b, color.a);
		ImColor pinInnerColor = ImColor(Inner_Color.r, Inner_Color.g, Inner_Color.b, alpha);

		ImGui::Widgets::Icon(ImVec2(Icon_Size.x, Icon_Size.y), iconType, Linked, pinColor, pinInnerColor);
	}
}