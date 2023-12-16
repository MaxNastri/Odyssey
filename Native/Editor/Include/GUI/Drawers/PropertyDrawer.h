#pragma once
#include <GUIElement.h>
#include <string>

namespace Odyssey
{
	class PropertyDrawer : public GUIElement
	{
	public:
		PropertyDrawer() = default;
		PropertyDrawer(const std::string& label)
			: m_Label(label)
		{

		}

	protected:
		std::string m_Label;
	};
}