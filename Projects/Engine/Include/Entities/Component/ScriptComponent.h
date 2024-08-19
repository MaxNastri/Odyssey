#pragma once
#include "AssetSerializer.h"
#include "ManagedObject.hpp"
#include "Type.hpp"
#include "GameObject.h"
#include "FileID.h"

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
		uint32_t GetScriptID() { return m_ScriptID; }
		Coral::Type GetType() { return managedInstance.GetType(); }
		Coral::ManagedObject GetManagedObject() { return managedInstance; }
		std::string GetManagedTypeName() { return m_ManagedType; }

	private:
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node);
		bool SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node);
		bool DeserializeNativeString(const std::string& fieldName, SerializationNode& node);
	
	private:
		FileID m_FileID;
		GameObject m_GameObject;
		Coral::ManagedObject managedInstance;
		std::string m_ManagedType;
		uint32_t m_ScriptID;
		CLASS_DECLARATION(ScriptComponent);
	};
}