#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "ScriptStorage.h"
#include "ManagedHandle.h"

namespace Odyssey
{
	class ScriptComponent
	{
	public:
		ScriptComponent() = default;
		ScriptComponent(const GameObject& gameObject);
		ScriptComponent(const GameObject& gameObject, SerializationNode& node, std::map<GUID, GUID>& remap);
		ScriptComponent(const GameObject& gameObject, const std::string& managedType);
		ScriptComponent(const GameObject& gameObject, uint32_t scriptID);

	public:
		void Awake();
		void Update();
		void OnDestroy();
		void OnCollisionEnter(GameObject& body, float3 contactNormal);
		void OnCollisionStay(GameObject& body, float3 contactNormal);
		void OnCollisionExit(GameObject& body);

	public:
		void Serialize(SerializationNode& node);
		void SerializeAsPrefab(SerializationNode& node, std::map<GUID, GUID>& remap);
		void DeserializeAsPrefab(SerializationNode& node, std::map<GUID, GUID>& remap);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetScriptID(uint32_t scriptID);
		void SetManagedHandle(ManagedHandle handle) { m_Handle = handle; }

	public:
		bool IsEnabled() { return m_Enabled; }
		uint32_t GetScriptID() { return m_ScriptID; }
		ManagedHandle& GetManagedHandle() { return m_Handle; }

	public:
		void ClearManagedHandle() { m_Handle.Clear(); }

	private:
		void SerializeNativeTypes(SerializationNode& node, FieldStorage& storage);
		bool SerializeNativeString(SerializationNode& node, FieldStorage& storage);
		void DeserializeNativeType(SerializationNode& node, FieldStorage& storage);
		bool DeserializeNativeString(SerializationNode& node, FieldStorage& storage);

	private:
		bool m_Enabled = true;
		uint32_t m_ScriptID;
		GameObject m_GameObject;
		ManagedHandle m_Handle;
		CLASS_DECLARATION(Odyssey, ScriptComponent)
	};
}