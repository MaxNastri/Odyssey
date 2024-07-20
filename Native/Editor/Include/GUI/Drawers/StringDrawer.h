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

	public:
		void SetData(std::string data) { m_Data = data; }

	private:
		std::string m_Data;
	};
	class StringDrawer : public PropertyDrawer
	{
	public:
		StringDrawer() = default;
		StringDrawer(const std::string& propertyLabel, std::string initialValue, std::function<void(std::string&)> callback, bool readOnly = false);

	public:
		virtual void Draw() override;

	private:
		std::function<void(std::string&)> valueUpdatedCallback;
		std::string data;
		bool m_ReadOnly = false;
	};
}