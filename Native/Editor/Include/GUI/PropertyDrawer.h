#pragma once
#include <GUIElement.h>
#include <imgui.h>
#include <string>
#include <functional>
#include <Vector4.h>
#include <array>

namespace Odyssey::Editor
{
	class PropertyDrawer
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

	class FloatDrawer : public PropertyDrawer, public Graphics::GUIElement
	{
	public:
		FloatDrawer(const std::string& propertyLabel, float initialValue)
			: PropertyDrawer(label), value(initialValue)
		{
		}

	public:
		virtual void Draw() override
		{
			if (ImGui::InputFloat(label.data(), &value, step, stepFast))
			{
				valueUpdatedCallback(value);
			}
		}

	private:
		std::function<void(float)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		float value;
	};

	class Vector4Drawer : public PropertyDrawer, public Graphics::GUIElement
	{
	public:
		Vector4Drawer() = default;
		Vector4Drawer(const std::string& propertyLabel, Vector4 vec4)
		{
			label = propertyLabel;
			data = { vec4.x, vec4.y, vec4.z, vec4.w };
		}

	public:
		virtual void Draw() override
		{
			if (ImGui::InputFloat4(label.data(), data.data()))
			{
				onValueModified(Vector4(data[0], data[1], data[2], data[3]));
			}
		}

		void SetCallback(std::function<void(Vector4)> callback)
		{
			onValueModified = callback;
		}

	private:
		std::function<void(Vector4)> onValueModified;
		std::array<float, 4> data{ 0.0f };
	};
}