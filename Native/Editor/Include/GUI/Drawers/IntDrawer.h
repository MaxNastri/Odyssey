#pragma once
#include "PropertyDrawer.h"
#include <imgui.h>

namespace Odyssey
{
	template <typename T>
	class IntDrawer : public PropertyDrawer
	{
	public:
		IntDrawer(const std::string& propertyLabel, T initialValue, std::function<void(T)> callback)
			: PropertyDrawer(propertyLabel)
		{
			data = initialValue;
			valueUpdatedCallback = callback;
			dataType = GetDataType();
		}

	public:
		virtual void Draw() override
		{
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(m_Label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::InputScalar(m_Label.data(), dataType, &data))
			{
				valueUpdatedCallback(data);
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
	};
}