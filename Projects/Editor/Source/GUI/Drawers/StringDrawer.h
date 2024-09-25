#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer() = default;
		StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(const std::string&)> callback, bool readOnly = false);

	public:
		virtual void Draw() override;

	public:
		const std::string& GetValue() { return m_Data; }

	private:
		std::function<void(const std::string&)> valueUpdatedCallback;
		std::string m_Data;
		bool m_ReadOnly = false;
	};
}