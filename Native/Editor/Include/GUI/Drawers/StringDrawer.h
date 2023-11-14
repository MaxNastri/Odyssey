#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(std::string)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(std::string)> valueUpdatedCallback;
		std::string data;
	};
}