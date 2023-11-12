#include "GameObject.h"
#include "Component.h"
#include "ComponentManager.h"

namespace Odyssey::Entities
{
    CLASS_DEFINITION(Odyssey.Entities, GameObject);
    GameObject::GameObject()
    {
        name = "GameObject";
        id = -1;
        active = false;
    }

    GameObject::GameObject(unsigned int ID)
    {
        name = "GameObject";
        id = ID;
        active = true;
    }

    void GameObject::Serialize(json& sceneJson)
    {
        json gameObjectJson;
        to_json(gameObjectJson, *this);

        auto serializeComponent = [&gameObjectJson](Component* component)
            {
                component->Serialize(gameObjectJson);
            };

        ComponentManager::ExecuteOnGameObjectComponents(*this, serializeComponent);
        sceneJson += {"GameObject." + std::to_string(id), gameObjectJson};
    }

    void GameObject::Deserialize(const json& jsonObject)
    {
        from_json(jsonObject, *this);

        for (auto& element : jsonObject)
        {
            if (element.is_object())
            {
                std::string type = element.at("Type");
                Component* component = nullptr;
                if (type == UserScript::Type)
                {
                    std::string managedType = element.at("ManagedType");
                    component = ComponentManager::AddUserScript(*this, managedType);
                }
                else
                {
                    component = ComponentManager::AddComponentByName(*this, type);
                }
                component->Deserialize(element);
            }
        }
    }
}