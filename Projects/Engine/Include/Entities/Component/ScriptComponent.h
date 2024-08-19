#pragma once
#include "AssetSerializer.h"
#include "ManagedObject.hpp"
#include "Type.hpp"
#include "GameObject.h"
#include "FileID.h"
#include "ScriptStorage.h"

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
		std::string GetManagedTypeName() { return m_ManagedType; }

	private:
		void SerializeNativeTypes(SerializationNode& node, FieldStorage& storage);
		bool SerializeNativeString(SerializationNode& node, FieldStorage& storage);
		void DeserializeNativeType(SerializationNode& node, FieldStorage& storage);
		bool DeserializeNativeString(SerializationNode& node, FieldStorage& storage);
	
	private:
		FileID m_FileID;
		GameObject m_GameObject;
		Coral::ManagedObject managedInstance;
		std::string m_ManagedType;
		uint32_t m_ScriptID;
		CLASS_DECLARATION(ScriptComponent);
	};
}