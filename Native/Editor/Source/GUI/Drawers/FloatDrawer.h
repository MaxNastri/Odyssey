#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class FloatDrawer : public PropertyDrawer
	{
	public:
		FloatDrawer() = default;
		FloatDrawer(const std::string& propertyLabel, float initialValue, std::function<void(float)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(float)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		float data;
	};
}