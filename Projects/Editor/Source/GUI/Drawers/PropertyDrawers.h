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
		AssetFieldDrawer(std::string_view label, GUID guid, const std::string& assetType, std::function<void(GUID)> callback);

	public:
		virtual void Draw() override;

	public:
		void SetGUID(GUID guid);
		GUID GetGUID() { return m_GUID; }

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
		BoolDrawer(std::string_view label, bool initialValue, std::function<void(bool)> callback, bool readOnly = false);

	public:
		virtual void Draw() override;
		void SetData(bool data) { m_Data = data; }
	private:
		std::function<void(bool)> valueUpdatedCallback;
		bool m_Data;
		bool m_ReadOnly = false;
	};

	class ColorPicker : public PropertyDrawer
	{
	public:
		ColorPicker() = default;
		ColorPicker(std::string_view propertyLabel, float3 initialValue, std::function<void(float3)> callback);
		ColorPicker(std::string_view propertyLabel, float4 initialValue, std::function<void(float4)> callback);

	public:
		virtual void Draw() override;

	public:
		void SetColor(float3 color) { m_Color = float4(color, 1.0f); }
		void SetColor(float4 color) { m_Color = color; }
		float4 GetColor3() { return m_Color; }
		float4 GetColor4() { return m_Color; }

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
		DoubleDrawer(std::string_view label, double initialValue, std::function<void(double)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(double)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		double value;
	};

	class EntityFieldDrawer : public PropertyDrawer
	{
	public:
		EntityFieldDrawer() = default;
		EntityFieldDrawer(std::string_view label, GUID guid, const std::string& typeName, std::function<void(GUID)> callback);

	public:
		virtual void Draw() override;

	private:
		void GeneratePossibleEntities();

	private:
		struct EntityData
		{
		public:
			std::string GameObjectName;
			GUID GameObjectGUID;
		};
		GUID m_GUID = 0;
		std::string m_TypeName;
		uint64_t m_SelectedIndex = 0;
		std::vector<EntityData> m_Entities;
		std::function<void(GUID)> m_OnValueModified;
	};

	template<typename T>
	class EnumDrawer : public PropertyDrawer
	{
	public:
		EnumDrawer() = default;
		EnumDrawer(std::string_view label, T initialValue, std::function<void(T)> callback)
			: m_EnumMenu(initialValue)
		{
			m_Label = label;
			m_OnValueModified = callback;
		}

	public:
		virtual void Draw() override
		{
			ImGui::PushID(this);

			if (ImGui::BeginTable("##Table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(m_Label.data());
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-0.01f);

				if (m_EnumMenu.Draw())
					m_OnValueModified(m_EnumMenu.GetValue());

				ImGui::EndTable();
			}

			ImGui::PopID();
		}

	private:
		std::function<void(T)> m_OnValueModified;
		EnumComboMenu<T> m_EnumMenu;
	};

	class FloatDrawer : public PropertyDrawer
	{
	public:
		FloatDrawer() = default;
		FloatDrawer(std::string_view label, float initialValue, std::function<void(float)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(float)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		float data;
	};

	template <typename T>
	class IntDrawer : public PropertyDrawer
	{
	public:
		IntDrawer() = default;
		IntDrawer(const std::string& propertyLabel, T initialValue, std::function<void(T)> callback, bool readOnly = false)
			: PropertyDrawer(propertyLabel)
		{
			data = initialValue;
			valueUpdatedCallback = callback;
			dataType = GetDataType();
			m_ReadOnly = readOnly;
		}

	public:
		virtual void Draw() override
		{
			if (ImGui::BeginTable("table", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
			{
				ImGui::TableSetupColumn("##empty", 0, m_LabelWidth);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(m_Label.data());
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-0.01f);
				if (m_ReadOnly)
					ImGui::BeginDisabled();

				if (ImGui::InputScalar(m_Label.data(), dataType, &data))
				{
					valueUpdatedCallback(data);
				}

				if (m_ReadOnly)
					ImGui::EndDisabled();
				ImGui::EndTable();
			}
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

		std::function<void(T)> valueUpdatedCallback;

	private:
		ImGuiDataType dataType;
		uint32_t step = 0;
		uint32_t stepFast = 0;
		T data;
		bool m_ReadOnly = false;
	};

	class RangeSlider : public PropertyDrawer
	{
	public:
		RangeSlider() = default;
		RangeSlider(const std::string& label, float2 range, float2 limits, float step, std::function<void(float2)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(float2)> m_ValueUpdatedCallback;
		float2 m_Range = float2(0.0f);
		float2 m_Limits = float2(0.0f);
		float m_Step = 0.1f;
	};

	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer() = default;
		StringDrawer(std::string_view label, std::string_view initialValue, std::function<void(std::string_view)> callback, bool readOnly = false);

	public:
		virtual void Draw() override;

	public:
		std::string_view GetValue() { return m_Data; }

	private:
		std::function<void(std::string_view)> valueUpdatedCallback;
		char m_Data[128] = "";
		bool m_ReadOnly = false;
	};

	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(std::string_view propertyLabel, float3 initialValue, float3 resetValue, bool drawButtons, std::function<void(float3)> callback);

	public:
		virtual void Draw() override;
		void SetValue(float3 value) { m_Data = value; }

	private:
		std::function<void(float3)> onValueModified;
		float3 m_Data;
		float3 m_ResetValue;
		bool m_DrawButtons = false;
	};
}