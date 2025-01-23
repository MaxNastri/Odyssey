#include "PropertiesComponent.h"

namespace Odyssey
{
	PropertiesComponent::PropertiesComponent(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
	}

	PropertiesComponent::PropertiesComponent(const GameObject& gameObject, Odyssey::GUID guid)
		: m_GameObject(gameObject), GUID(guid)
	{
	}

	PropertiesComponent::PropertiesComponent(const GameObject& gameObject, Odyssey::GUID guid, const std::string& name)
		: m_GameObject(gameObject), GUID(guid), Name(name)
	{
	}
}