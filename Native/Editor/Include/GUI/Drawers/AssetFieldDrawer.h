#pragma once
#include "PropertyDrawer.h"

namespace Odyssey
{
	class AssetFieldDrawer : public PropertyDrawer
	{
	public:
		AssetFieldDrawer() = default;
		AssetFieldDrawer(const std::string& label, const std::string& guid, const std::string& assetType, std::function<void(const std::string&)> callback);

	public:
		virtual void Draw() override;

	private:
		std::function<void(const std::string&)> m_OnValueModified;
		std::string m_GUID;
		std::string m_Type;
		int32_t selectedIndex = 0;
	};
}