#pragma once
#include "GUIElement.h"

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
		void SetLabelWidth(float width) { m_LabelWidth = width; }

	protected:
		std::string m_Label;
		float m_LabelWidth = 0.5f;
		bool m_Modified = false;
	};
}