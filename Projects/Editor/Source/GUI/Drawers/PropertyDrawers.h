#pragma once
#include "GUIElement.h"
#include "GUID.h"
#include "imgui.h"
#include "EditorWidgets.h"

namespace Odyssey
{
	class PropertyDrawer : public GUIElement
	{
	public:
		PropertyDrawer() = default;
		PropertyDrawer(std::string_view label) : m_Label(label) { }

	public:
		void SetLabelWidth(float width) { m_LabelWidth = width; }

	protected:
		std::string m_Label;
		float m_LabelWidth = 0.5f;
	};

	class AssetFieldDrawer : public PropertyDrawer
	{
	public:
		AssetFieldDrawer() = default;
		AssetFieldDrawer(std::string_view label, GUID guid, const std::string& assetType, std::function<void(GUID)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		GUID GetGUID() { return m_GUID; }
		void SetGUID(GUID guid);

	private:
		void SetSelectedIndex();

	private:
		std::function<void(GUID)> m_OnValueModified;
		GUID m_GUID;
		std::string m_Type;
		int32_t selectedIndex = 0;
	};

	class BoolDrawer : public PropertyDrawer
	{
	public:
		BoolDrawer() = default;
		BoolDrawer(std::string_view label, bool initialValue, bool readOnly = false, std::function<void(bool)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		bool GetValue() { return m_Value; }
		void SetValue(bool value) { m_Value = value; }

	private:
		bool m_Value;
		bool m_ReadOnly = false;
		std::function<void(bool)> m_OnValueModifed;
	};

	class ColorPicker : public PropertyDrawer
	{
	public:
		ColorPicker() = default;
		ColorPicker(std::string_view propertyLabel, float3 initialValue, std::function<void(float3)> callback = nullptr);
		ColorPicker(std::string_view propertyLabel, float4 initialValue, std::function<void(float4)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		float4 GetColor3() { return m_Color; }
		float4 GetColor4() { return m_Color; }
		void SetColor(float3 color) { m_Color = float4(color, 1.0f); }
		void SetColor(float4 color) { m_Color = color; }

	private:
		enum class ColorType
		{
			None = 0,
			Color3 = 1,
			Color4 = 2,
		};

	private:
		std::function<void(float3)> m_OnColor3Modified;
		std::function<void(float4)> m_OnColor4Modified;
		float4 m_Color;
		ColorType m_ColorType;
	};

	class DoubleDrawer : public PropertyDrawer
	{
	public:
		DoubleDrawer() = default;
		DoubleDrawer(std::string_view label, double initialValue, std::function<void(double)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		double GetValue() { return m_Value; }
		void SetValue(double value) { m_Value = value; }

	private:
		double m_Value;
		float m_Step = 0.0f;
		float m_StepFast = 0.0f;
		std::function<void(double)> m_OnValueModifed;
	};

	class DropdownDrawer : public PropertyDrawer
	{
	public:
		DropdownDrawer() = default;
		DropdownDrawer(std::string_view label, const std::vector<std::string>& options, uint64_t startIndex, std::function<void(std::string_view, uint64_t)> callback = nullptr);
		DropdownDrawer(std::string_view label, const std::vector<std::string>& options, std::string_view initialValue, std::function<void(std::string_view, uint64_t)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		std::string_view GetSelected();
		uint64_t GetSelectedIndex();

	public:
		void SetOptions(const std::vector<std::string>& options);

	private:
		Dropdown m_Dropdown;
		std::function<void(std::string_view, uint64_t)> m_OnValueModified;
	};

	class EntityFieldDrawer : public PropertyDrawer
	{
	public:
		EntityFieldDrawer() = default;
		EntityFieldDrawer(std::string_view label, GUID guid, const std::string& typeName, std::function<void(GUID)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		GUID GetEntity() { return m_Dropdown.GetEntity(); }

	private:
		EntityDropdown m_Dropdown;
		std::function<void(GUID)> m_OnValueModified;
	};

	template<typename T>
	class EnumDrawer : public PropertyDrawer
	{
	public:
		EnumDrawer() = default;
		EnumDrawer(std::string_view label, T initialValue, std::function<void(T)> callback = nullptr)
			: m_EnumMenu(initialValue)
		{
			m_Label = label;
			m_OnValueModified = callback;
		}

	public:
		T GetValue() { return m_EnumMenu.GetValue(); }

	public:
		virtual bool Draw() override
		{
			bool modified = false;

			ImGui::PushID(this);

			if (ImGui::BeginTable("##Table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(m_Label.data());
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-0.01f);

				if (m_EnumMenu.Draw())
				{
					modified = true;

					if (m_OnValueModified)
						m_OnValueModified(m_EnumMenu.GetValue());
				}

				ImGui::EndTable();
			}

			ImGui::PopID();

			return modified;
		}

	private:
		EnumDropdown<T> m_EnumMenu;
		std::function<void(T)> m_OnValueModified;
	};

	class FloatDrawer : public PropertyDrawer
	{
	public:
		FloatDrawer() = default;
		FloatDrawer(std::string_view label, float initialValue, std::function<void(float)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		void SetValue(float value) { m_Value = value; }
		float GetValue() { return m_Value; }

	private:
		float m_Value = 0.0f;
		float m_Step = 0.0f;
		float m_StepFast = 0.0f;
		std::function<void(float)> m_OnValueModifed;
	};

	template <typename T>
	class IntDrawer : public PropertyDrawer
	{
	public:
		IntDrawer() = default;
		IntDrawer(const std::string& propertyLabel, T initialValue, bool readOnly = false, std::function<void(T)> callback = nullptr)
			: PropertyDrawer(propertyLabel)
		{
			m_Value = initialValue;
			m_DataType = GetDataType();
			m_ReadOnly = readOnly;
			m_OnValueModifed = callback;
		}

	public:
		T GetValue() { return m_Value; }

	public:
		virtual bool Draw() override
		{
			bool modified = false;

			ImGui::PushID(this);

			if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(m_Label.data());
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-0.01f);

				if (m_ReadOnly)
					ImGui::BeginDisabled();

				if (ImGui::InputScalar(m_Label.data(), m_DataType, &m_Value))
				{
					modified = true;

					if (m_OnValueModifed)
						m_OnValueModifed(m_Value);
				}

				if (m_ReadOnly)
					ImGui::EndDisabled();

				ImGui::EndTable();
			}

			ImGui::PopID();

			return modified;
		}

	private:
		ImGuiDataType GetDataType()
		{
			if constexpr (std::same_as<T, uint8_t> || std::same_as<T, std::byte>)
				return ImGuiDataType_::ImGuiDataType_U8;
			else if constexpr (std::same_as<T, uint16_t>)
				return ImGuiDataType_::ImGuiDataType_U16;
			else if constexpr (std::same_as<T, uint32_t> || (std::same_as<T, unsigned long> && sizeof(T) == 4))
				return ImGuiDataType_::ImGuiDataType_U32;
			else if constexpr (std::same_as<T, uint64_t> || (std::same_as<T, unsigned long> && sizeof(T) == 8))
				return ImGuiDataType_::ImGuiDataType_U64;
			else if constexpr (std::same_as<T, char8_t>)
				return ImGuiDataType_::ImGuiDataType_S8;
			else if constexpr (std::same_as<T, int16_t>)
				return ImGuiDataType_::ImGuiDataType_S16;
			else if constexpr (std::same_as<T, int32_t> || (std::same_as<T, long> && sizeof(T) == 4))
				return ImGuiDataType_::ImGuiDataType_S32;
			else if constexpr (std::same_as<T, int64_t> || (std::same_as<T, long> && sizeof(T) == 8))
				return ImGuiDataType_::ImGuiDataType_S64;
			return ImGuiDataType_::ImGuiDataType_S64;
		}

	private:
		T m_Value;
		ImGuiDataType m_DataType;
		bool m_ReadOnly = false;
		std::function<void(T)> m_OnValueModifed;
	};

	class RangeSlider : public PropertyDrawer
	{
	public:
		RangeSlider() = default;
		RangeSlider(const std::string& label, float2 range, float2 limits, float step, std::function<void(float2)> callback = nullptr);

	public:
		virtual bool Draw() override;

	private:
		float2 m_Range = float2(0.0f);
		float2 m_Limits = float2(0.0f);
		float m_Step = 0.1f;
		std::function<void(float2)> m_OnValueModifed;
	};

	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer() = default;
		StringDrawer(std::string_view label, std::string_view initialValue, bool readOnly = false, std::function<void(std::string_view)> callback = nullptr);

	public:
		virtual bool Draw() override;

	public:
		std::string_view GetValue() { return m_Value; }

	private:
		char m_Value[128] = "";
		bool m_ReadOnly = false;
		std::function<void(std::string_view)> m_OnValueModifed;
	};

	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(std::string_view propertyLabel, float3 initialValue, float3 resetValue, bool drawButtons, std::function<void(float3)> callback = nullptr);

	public:
		float3 GetValue() { return m_Value; }
		void SetValue(float3 value) { m_Value = value; }

	public:
		virtual bool Draw() override;

	private:
		float3 m_Value;
		float3 m_ResetValue;
		bool m_DrawButtons = false;
		std::function<void(float3)> onValueModified;
	};
}