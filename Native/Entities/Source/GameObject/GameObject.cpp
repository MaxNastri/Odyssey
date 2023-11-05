#include "GameObject.h"
#include "Component.h"
#include "ComponentManager.h"

namespace Odyssey::Entities
{
    GameObject::GameObject()
    {
        id = -1;
        active = false;
    }

    GameObject::GameObject(unsigned int ID)
    {
        id = ID;
        active = true;
    }

    void GameObject::Serialize(json& jsonObject)
    {
        to_json(jsonObject, *this);

        auto serializeComponent = [&jsonObject](Component* component)
            {
                component->Serialize(jsonObject);
            };

        ComponentManager::ExecuteOnGameObjectComponents(*this, serializeComponent);
    }

    void GameObject::Deserialize(const json& jsonObject)
    {
    }
}