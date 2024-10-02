#pragma once
#include "PropertyDrawer.h"
namespace Odyssey
{
	class Color3Drawer : public PropertyDrawer
	{
	public:
		Color3Drawer() = default;
		Color3Drawer(std::string_view propertyLabel, float3 initialValue, std::function<void(float3)> callback);

	public:
		virtual void Draw() override;
		void SetValue(float3 value) { m_Data = value; }

	private:
		std::function<void(float3)> onValueModified;
		float3 m_Data;
	};

	class Color4Drawer : public PropertyDrawer
	{
	public:
		Color4Drawer() = default;
		Color4Drawer(std::string_view propertyLabel, float4 initialValue, std::function<void(float4)> callback);

	public:
		virtual void Draw() override;
		void SetValue(float4 value) { m_Data = value; }

	private:
		std::function<void(float4)> onValueModified;
		float4 m_Data;
	};

}