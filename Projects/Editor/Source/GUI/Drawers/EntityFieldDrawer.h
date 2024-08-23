#pragma once
#include "PropertyDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class EntityFieldDrawer : public PropertyDrawer
	{
	public:
		EntityFieldDrawer() = default;
		EntityFieldDrawer(std::string_view label, GUID guid, std::function<void(GUID)> callback);

	public:
		virtual void Draw() override;

	private:
		void GeneratePossibleGUIDs();

	private:
		std::function<void(GUID)> m_OnValueModified;
		GUID m_GUID = 0;
		uint64_t m_SelectedIndex = 0;
		std::vector<GUID> m_PossibleGUIDs;
	};
}