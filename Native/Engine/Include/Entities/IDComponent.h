#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class IDComponent
	{
	public:
		IDComponent() = default;
		IDComponent(const GameObject& gameObject);
		IDComponent(const GameObject& gameObject, const std::string& guid);
		IDComponent(const GameObject& gameObject, const std::string& guid, const std::string& name);

	public:
		std::string GUID;
		std::string Name;

	private:
		GameObject m_GameObject;
		CLASS_DECLARATION(IDComponent);
	};
}