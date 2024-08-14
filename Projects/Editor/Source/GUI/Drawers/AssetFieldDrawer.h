#pragma once
#include "PropertyDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class AssetFieldDrawer : public PropertyDrawer
	{
	public:
		AssetFieldDrawer() = default;
		AssetFieldDrawer(const std::string& label, GUID guid, const std::string& assetType, std::function<void(GUID)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(GUID)> m_OnValueModified;
		GUID m_GUID;
		std::string m_Type;
		int32_t selectedIndex = 0;
	};
}