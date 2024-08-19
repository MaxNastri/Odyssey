#pragma once
#include "PropertyDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class EntityFieldDrawer : public PropertyDrawer
	{
	public:
		EntityFieldDrawer() = default;
		EntityFieldDrawer(const std::string& label, GUID guid, std::function<void(GUID)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(GUID)> m_OnValueModified;
		GUID m_GUID;
		int32_t selectedIndex = 0;
	};
}