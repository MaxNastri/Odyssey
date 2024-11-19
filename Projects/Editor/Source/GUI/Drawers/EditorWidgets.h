#pragma once
#include "GUIElement.h"
#include "imgui.h"
#include "Light.h"
#include "Enum.hpp"

namespace Odyssey
{
	template<typename T>
	class EnumComboMenu
	{
	public:
		EnumComboMenu() = default;
		EnumComboMenu(T initialValue)
		{
			m_DisplayNames = Enum::GetNameSequence<T>();
			m_Value = initialValue;
			m_Selected = Enum::ToInt<T>(m_Value);
		}

		bool Draw()
		{
			bool modified = false;

			ImGui::PushID(this);

			std::string_view selectedName = m_DisplayNames[m_Selected];

			if (ImGui::BeginCombo("##Label", selectedName.data()))
			{
				for (size_t i = 0; i < m_DisplayNames.size(); i++)
				{
					const bool isSelected = m_Selected == i;
					std::string_view name = m_DisplayNames[i];

					if (ImGui::Selectable(name.data(), isSelected))
					{
						m_Value = Enum::ToEnum<LightType>(name);
						m_Selected = i;
						modified = true;
						break;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return modified;
		}

		T GetValue() { return m_Value; }

	private:
		T m_Value;
		std::vector<std::string> m_DisplayNames;
		size_t m_Selected = 0;
	};
}