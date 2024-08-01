#pragma once
#include "ComponentArray.h"
#include "UserScript.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

namespace Odyssey
{
	class ComponentRegistry
	{
	public: // Public templates
		template<typename... Args>
		Component* AddComponentByName(const int32_t id, const std::string& fqName, Args&&... params)
		{
			if (fqName == Transform::Type)
			{
				return AddComponent<Transform>(id, params...);
			}
			else if (fqName == Camera::Type)
			{
				return AddComponent<Camera>(id, params...);
			}
			else if (fqName == MeshRenderer::Type)
			{
				return AddComponent<MeshRenderer>(id, params...);
			}
			else if (fqName == UserScript::Type)
			{
				return AddUserScript(id, fqName, params...);
			}
			return nullptr;
		}

		template<typename T, typename... Args>
		T* AddComponent(const int32_t id, Args&&... params)
		{
			std::type_index typeID = typeid(T);
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			unsigned int index = componentArray->InsertData(id, params...);

			if (index != -1)
			{
				gameObjectToComponentArrayIndex[id].push_back(ComponentArrayEntry(typeID, index));
			}

			return componentArray->GetComponentData(id);
		}

		template<typename... Args>
		UserScript* AddUserScript(const int32_t id, const std::string& managedName, Args&&... params)
		{
			ComponentArray<UserScript>* userScriptArray = GetUserScriptArray(managedName);
			uint32_t index = userScriptArray->InsertData(id, params...);
			if (index != -1)
			{
				gameObjectToUserScriptIndex[id].push_back(UserScriptArrayEntry(managedName, index));
			}

			UserScript* userScript = userScriptArray->GetComponentData(id);
			if (userScript)
			{
				userScript->SetManagedType(managedName);
			}

			return userScript;
		}

		template<typename T>
		void RemoveComponent(const int32_t id)
		{
			if (ComponentArray<T>* componentArray = GetComponentArray<T>())
			{
				componentArray->RemoveGameObject(id);
			}

			std::type_index typeID = typeid(T);

			for (int i = 0; i < gameObjectToComponentArrayIndex[id].size(); ++i)
			{
				auto& entry = gameObjectToComponentArrayIndex[id][i];

				if (entry.Type == typeID)
				{
					gameObjectToComponentArrayIndex[id].erase(gameObjectToComponentArrayIndex[id].begin() + i);

					if (gameObjectToComponentArrayIndex[id].size() == 0)
					{
						gameObjectToComponentArrayIndex.erase(id);
					}

					break;
				}
			}
		}

		template<typename T>
		T* GetComponent(const int32_t id)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetComponentData(id);
		}

		template<typename T>
		bool HasComponent(const int32_t id)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->HasComponent(id);
		}

	private: // Private templates
		template<typename T>
		ComponentArray<T>* GetComponentArray()
		{
			std::type_index typeIndex = typeid(T);
			if (componentArrays.find(typeIndex) == componentArrays.end())
			{
				componentArrays[typeIndex] = std::make_unique<ComponentArray<T>>();
			}
			return static_cast<ComponentArray<T>*>(componentArrays[typeIndex].get());
		}

	public:
		void RemoveUserScript(const int32_t id, const std::string& managedName);
		UserScript* GetUserScript(const int32_t id, const std::string& managedName);
		std::vector<UserScript*> GetAllUserScripts(const int32_t id);

		bool HasUserScript(const int32_t id, const std::string& managedName);

		void ExecuteOnGameObjectComponents(const int32_t id, std::function<void(Component*)> func);
		void RemoveGameObject(const int32_t id);

	private:
		ComponentArray<UserScript>* GetUserScriptArray(const std::string& managedName);

	private:
		struct ComponentArrayEntry
		{
		public:
			ComponentArrayEntry() = default;
			ComponentArrayEntry(std::type_index type, uint32_t index) : Type(type), Index(index) { }

		public:
			std::type_index Type;
			uint32_t Index;
		};

		struct UserScriptArrayEntry
		{
		public:
			UserScriptArrayEntry() = default;
			UserScriptArrayEntry(std::string type, uint32_t index) : Type(type), Index(index) { }

		public:
			std::string Type;
			uint32_t Index;
		};

		// [Component Type, Component Array]
		std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
		// [GameObject ID, List<Component Entry>]
		std::unordered_map<int32_t, std::vector<ComponentArrayEntry>> gameObjectToComponentArrayIndex;

		// [UserScript Name, Componen tArray]
		std::unordered_map<std::string, std::unique_ptr<ComponentArray<UserScript>>> userScriptArrays;
		// [GameObject ID, List<UserScripts>]
		std::unordered_map<int32_t, std::vector<UserScriptArrayEntry>> gameObjectToUserScriptIndex;
	};
};