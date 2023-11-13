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

        auto userScripts = ComponentManager::GetAllUserScripts(*this);

        for (auto& [scriptName, userScript] : userScripts)
        {
            userScript->Serialize(gameObjectJson);
        }

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
                    component = ComponentManager::AddUserScript(*this, managedType, managedType);
                }
                else
                {
                    component = ComponentManager::AddComponentByName(*this, type);
                }
                component->Deserialize(element);
            }
        }
    }

    void GameObject::Serialize(ryml::NodeRef& node)
    {
        ryml::NodeRef gameObjectNode = node.append_child();
        gameObjectNode |= ryml::MAP;
        gameObjectNode["Name"] << name;
        gameObjectNode["Type"] << Type;
        gameObjectNode["Active"] << active;
        gameObjectNode["ID"] << id;

        ryml::NodeRef componentsNode = gameObjectNode["Components"];
        componentsNode |= ryml::SEQ;

        //Transform* transform = ComponentManager::GetComponent<Transform>(*this);
        //transform->Serialize(gameObjectNode);
        //auto serializeComponent = [&node](Component* component)
        //    {
        //        component->Serialize(node);
        //    };
        //
        //ComponentManager::ExecuteOnGameObjectComponents(*this, serializeComponent);

        auto userScripts = ComponentManager::GetAllUserScripts(*this);
        
        for (auto& [scriptName, userScript] : userScripts)
        {
            userScript->Serialize(componentsNode);
        }
    }
}