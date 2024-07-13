#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class ReadOnlyStringDrawer : public PropertyDrawer
	{
	public:
		ReadOnlyStringDrawer() = default;
		ReadOnlyStringDrawer(const std::string& propertyLabel, std::string initialValue);

	public:
		virtual void Draw() override;

	private:
		std::string data;
	};
	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer() = default;
		StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(std::string)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(std::string)> valueUpdatedCallback;
		std::string data;
	};
}