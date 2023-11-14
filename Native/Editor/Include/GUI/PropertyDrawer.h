#pragma once
#include <GUIElement.h>
#include <string>

namespace Odyssey::Editor
{
	class PropertyDrawer : public GUIElement
	{
	public:
		PropertyDrawer() = default;
		PropertyDrawer(const std::string& label)
			: label(label)
		{

		}

	protected:
		std::string label;
	};
}