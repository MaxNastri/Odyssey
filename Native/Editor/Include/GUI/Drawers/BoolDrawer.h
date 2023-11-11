#pragma once
#include "PropertyDrawer.h"

namespace Odyssey::Editor
{
	class BoolDrawer : public PropertyDrawer
	{
	public:
		BoolDrawer(const std::string& propertyLabel, bool initialValue);

	public:
		virtual void Draw() override;
		void SetCallback(std::function<void(bool)> callback);

	private:
		std::function<void(bool)> valueUpdatedCallback;
		bool data;
	};
}