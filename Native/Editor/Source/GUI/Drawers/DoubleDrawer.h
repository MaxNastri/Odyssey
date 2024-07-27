#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class DoubleDrawer : public PropertyDrawer
	{
	public:
		DoubleDrawer(const std::string& propertyLabel, double initialValue, std::function<void(double)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(double)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		double value;
	};
}