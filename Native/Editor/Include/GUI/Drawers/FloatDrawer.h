#pragma once
#include "PropertyDrawer.h"

namespace Odyssey::Editor
{
	class FloatDrawer : public PropertyDrawer
	{
	public:
		FloatDrawer(const std::string& propertyLabel, float initialValue);			

	public:
		virtual void Draw() override;
		void SetCallback(std::function<void(float)> callback);

	private:
		std::function<void(float)> valueUpdatedCallback;
		float step = 0.0f;
		float stepFast = 0.0f;
		float value;
	};
}