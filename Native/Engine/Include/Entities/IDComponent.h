#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "GUID.h"

namespace Odyssey
{
	class IDComponent
	{
	public:
		IDComponent() = default;
		IDComponent(const GameObject& gameObject);
		IDComponent(const GameObject& gameObject, GUID guid);
		IDComponent(const GameObject& gameObject, GUID guid, const std::string& name);

	public:
		GUID GUID;
		std::string Name;

	private:
		GameObject m_GameObject;
		CLASS_DECLARATION(IDComponent);
	};
}