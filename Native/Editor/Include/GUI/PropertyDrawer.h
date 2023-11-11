#pragma once
#include <GUIElement.h>
#include <imgui.h>
#include <string>
#include <functional>
#include <Vector4.h>
#include <array>

namespace Odyssey::Editor
{
	class PropertyDrawer : public Graphics::GUIElement
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

	class FloatDrawer : public PropertyDrawer
	{
	public:
		FloatDrawer(const std::string& propertyLabel, float initialValue)
			: PropertyDrawer(propertyLabel), value(initialValue)
		{
		}

	public:
		virtual void Draw() override
		{
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::InputFloat(label.data(), &value, step, stepFast))
			{
				valueUpdatedCallback(value);
			}
		}

		void SetCallback(std::function<void(float)> callback)
		{
			valueUpdatedCallback = callback;
		}

	private:
		std::function<void(float)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		float value;
	};

	class Vector3Drawer : public PropertyDrawer
	{
	public:
		Vector3Drawer() = default;
		Vector3Drawer(const std::string& propertyLabel, Vector3 vec3)
		{
			label = propertyLabel;
			data = { vec3.x, vec3.y, vec3.z };
		}

	public:
		virtual void Draw() override
		{
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(label.data());
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-0.01f);
			if (ImGui::InputFloat3(label.data(), data.data()))
			{
				onValueModified(Vector3(data[0], data[1], data[2]));
			}
		}

		void SetCallback(std::function<void(Vector3)> callback)
		{
			onValueModified = callback;
		}

	private:
		std::function<void(Vector3)> onValueModified;
		std::array<float, 3> data{ 0.0f };
	};
}