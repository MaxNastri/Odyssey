#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "GUID.h"

namespace Odyssey
{
	class PropertiesComponent
	{
	public:
		PropertiesComponent() = default;
		PropertiesComponent(const GameObject& gameObject);
		PropertiesComponent(const GameObject& gameObject, GUID guid);
		PropertiesComponent(const GameObject& gameObject, GUID guid, const std::string& name);

	public:
		GUID GUID;
		std::string Name = "GameObject";
		bool Serialize = true;
		bool IsPrefabInstance = false;
		int64_t SortOrder = -1;

	private:
		GameObject m_GameObject;
		CLASS_DECLARATION(Odyssey, PropertiesComponent)
	};
}