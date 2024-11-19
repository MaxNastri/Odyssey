#pragma once
#include "GUIElement.h"
#include "imgui.h"
#include "GUID.h"
#include "Enum.h"

namespace Odyssey
{
	template<typename T>
	class EnumDropdown
	{
	public:
		EnumDropdown() = default;
		EnumDropdown(T initialValue)
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
						m_Value = Enum::ToEnum<T>(name);
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

	class EntityDropdown
	{
	public:
		EntityDropdown(GUID initialValue, const std::string& typeName);

	public:
		bool Draw();

	public:
		GUID GetEntity() { return m_SelectedEntity; }

	private:
		void GeneratePossibleEntities();

	private:
		struct EntityData
		{
		public:
			std::string GameObjectName;
			GUID GameObjectGUID;
		};

		std::string m_TypeName;
		std::vector<EntityData> m_Entities;
		GUID m_SelectedEntity;
		uint64_t m_SelectedIndex = 0;
	};
}