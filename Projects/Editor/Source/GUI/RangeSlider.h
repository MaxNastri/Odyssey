#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
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
}