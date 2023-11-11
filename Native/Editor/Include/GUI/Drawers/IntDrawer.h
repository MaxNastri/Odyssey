#pragma once
#include "PropertyDrawer.h"

namespace Odyssey::Editor
{
	class IntDrawer : public PropertyDrawer
	{
	public:
		IntDrawer(const std::string& propertyLabel, uint32_t initialValue);

	public:
		virtual void Draw() override;
		void SetCallback(std::function<void(uint32_t)> callback);
	private:
		std::function<void(uint32_t)> valueUpdatedCallback;

	private:
		uint32_t step = 0;
		uint32_t stepFast = 0;
		uint32_t data;
	};
}