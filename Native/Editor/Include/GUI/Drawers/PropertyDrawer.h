#pragma once
#include <GUIElement.h>
#include <string>

namespace Odyssey
{
	class PropertyDrawer : public GUIElement
	{
	public:
		PropertyDrawer() = default;
		PropertyDrawer(const std::string& label) : m_Label(label) { }

	public:
		bool IsModified() { return m_Modified; }
		void SetModified(bool modified) { m_Modified = modified; }

	protected:
		std::string m_Label;
		bool m_Modified = false;
	};
}