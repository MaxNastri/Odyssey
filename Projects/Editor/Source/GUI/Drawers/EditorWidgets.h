#pragma once
#include "GUIElement.h"
#include "imgui.h"
#include "GUID.h"
#include "Enum.h"

namespace Odyssey
{
	class Dropdown
	{
	public:
		Dropdown() = default;
		Dropdown(const std::vector<std::string>& options);
		Dropdown(const std::vector<std::string>& options, uint64_t initialIndex);
		Dropdown(const std::vector<std::string>& options, std::string_view initialValue);

	public:
		bool Draw();

	public:
		void SetOptions(const std::vector<std::string>& options);

	public:
		std::string_view GetSelectedString() { return m_Options[m_SelectedIndex]; }
		uint64_t GetSelectedIndex() { return m_SelectedIndex; }

	private:
		std::vector<std::string> m_Options;
		uint64_t m_SelectedIndex = 0;
	};

	template<typename T>
	class EnumDropdown
	{
	public:
		EnumDropdown() = default;
		EnumDropdown(T initialValue)
		{
			std::vector<std::string> options = Enum::GetNameSequence<T>();
			m_Dropdown = Dropdown(options, Enum::ToInt<T>(initialValue));
		}

		bool Draw()
		{
			if (m_Dropdown.Draw())
			{
				m_Value = Enum::ToEnum<T>(m_Dropdown.GetSelectedIndex());
				return true;
			}

			return false;
		}

		T GetValue() { return m_Value; }

	private:
		T m_Value;
		Dropdown m_Dropdown;
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

	class SelectableInput
	{
	public:
		enum class Result
		{
			None = 0,
			Selected = 1,
			DoubleClick = 2,
			TextModified = 3,
		};

	public:
		SelectableInput() = default;
		SelectableInput(std::string_view text, uint64_t iconHandle = 0);

	public:
		Result Draw();
		std::string_view GetText() { return m_Text; }

	public:
		void SetIcon(uint64_t iconHandle) { m_IconHandle = iconHandle; }

	private:
		void DrawInput();

	private:
		std::string m_Text;
		char m_InputBuffer[128] = "";
		bool m_Selected = false;
		bool m_ShowInput = false;
		uint64_t m_IconHandle = 0;
		Result m_Result = Result::None;
	};
}