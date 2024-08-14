#pragma once
#include "AssetSerializer.h"
#include "ManagedObject.hpp"
#include "Type.hpp"
#include "GameObject.h"

namespace Odyssey
{
	class ScriptComponent 
	{
	public:
		ScriptComponent() = default;
		ScriptComponent(const GameObject& gameObject);
		ScriptComponent(const GameObject& gameObject, const std::string& managedType);
		void Awake();
		void Update();
		void OnDestroy();
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		Coral::Type GetType() { return managedInstance.GetType(); }
		Coral::ManagedObject GetManagedObject() { return managedInstance; }
		std::string GetManagedTypeName() { return m_ManagedType; }

	public:
		void SetManagedInstance(Coral::ManagedObject instance);
		void SetManagedType(std::string_view managedClassName);

	private:
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node);
		bool SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node);
		bool DeserializeNativeString(const std::string& fieldName, SerializationNode& node);
	
	private:
		GameObject m_GameObject;
		Coral::ManagedObject managedInstance;
		std::string m_ManagedType;
		CLASS_DECLARATION(ScriptComponent);
	};
}