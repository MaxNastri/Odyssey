#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class BoolDrawer : public PropertyDrawer
	{
	public:
		BoolDrawer() = default;
		BoolDrawer(const std::string& propertyLabel, bool initialValue, std::function<void(bool)> callback, bool readOnly = false);

	public:
		virtual void Draw() override;
		void SetData(bool data) { m_Data = data; }
	private:
		std::function<void(bool)> valueUpdatedCallback;
		bool m_Data;
		bool m_ReadOnly = false;
	};
}