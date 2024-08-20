#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "FileID.h"
#include "ScriptStorage.h"
#include "ManagedHandle.h"

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
		void SetScriptID(uint32_t scriptID);
		uint32_t GetScriptID() { return m_ScriptID; }
		void SetManagedHandle(ManagedHandle handle) { m_Handle = handle; }

	private:
		void SerializeNativeTypes(SerializationNode& node, FieldStorage& storage);
		bool SerializeNativeString(SerializationNode& node, FieldStorage& storage);
		void DeserializeNativeType(SerializationNode& node, FieldStorage& storage);
		bool DeserializeNativeString(SerializationNode& node, FieldStorage& storage);

	private:
		FileID m_FileID;
		uint32_t m_ScriptID;
		GameObject m_GameObject;
		ManagedHandle m_Handle;
		CLASS_DECLARATION(ScriptComponent);
	};
}