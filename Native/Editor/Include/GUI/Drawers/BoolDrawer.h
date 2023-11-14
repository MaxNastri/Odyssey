#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class BoolDrawer : public PropertyDrawer
	{
	public:
		BoolDrawer(const std::string& propertyLabel, bool initialValue, std::function<void(bool)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(bool)> valueUpdatedCallback;
		bool data;
	};
}