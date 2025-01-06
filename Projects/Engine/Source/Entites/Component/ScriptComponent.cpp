#include "ScriptComponent.h"
#include "ScriptingManager.h"
#include "String.hpp"

namespace Odyssey
{
	ScriptComponent::ScriptComponent(const GameObject& gameObject, const std::string& managedType)
		: m_GameObject(gameObject)
	{

	}

	ScriptComponent::ScriptComponent(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
	}

	void ScriptComponent::Awake()
	{
		if (m_Handle.IsValid())
			m_Handle.Invoke("Void Awake()");
	}

	void ScriptComponent::Update()
	{
		if (m_Handle.IsValid())
			m_Handle.Invoke("Void Update()");
	}

	void ScriptComponent::OnDestroy()
	{
		if (m_Handle.IsValid())
			m_Handle.Invoke("Void OnDestroy()");
	}

	void ScriptComponent::Serialize(SerializationNode& node)
	{
		// Get the fields from that type
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("m_ScriptID", m_ScriptID);
		componentNode.WriteData("Type", ScriptComponent::Type);

		SerializationNode fieldsNode = componentNode.CreateSequenceNode("Fields");

		// Make sure the field still exists on deserialization
		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::String)
			{
				// Do stuff
				SerializeNativeString(fieldsNode, fieldStorage);
			}
			else
			{
				SerializeNativeTypes(fieldsNode, fieldStorage);
			}
		}
	}

	void ScriptComponent::SerializeAsPrefab(SerializationNode& node, std::map<GUID, GUID>& remap)
	{
		// Get the fields from that type
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("m_ScriptID", m_ScriptID);
		componentNode.WriteData("Type", ScriptComponent::Type);

		SerializationNode fieldsNode = componentNode.CreateSequenceNode("Fields");

		// Make sure the field still exists on deserialization
		for (auto& [fieldID, fieldStorage] : storage.Fields)
		{
			if (fieldStorage.DataType == DataType::String)
			{
				// Do stuff
				SerializeNativeString(fieldsNode, fieldStorage);
			}
			else
			{
				// Important: We remap any field storage that contains an old guid reference
				if (fieldStorage.DataType == DataType::Entity ||
					fieldStorage.DataType == DataType::Component ||
					fieldStorage.DataType == DataType::Mesh ||
					fieldStorage.DataType == DataType::Material ||
					fieldStorage.DataType == DataType::Texture2D)
				{
					GUID value = fieldStorage.GetValue<GUID>();
					if (remap.contains(value))
					{
						value = remap[value];
						fieldStorage.SetValue<GUID>(value);
					}
				}
				SerializeNativeTypes(fieldsNode, fieldStorage);
			}
		}
	}

	void ScriptComponent::DeserializeAsPrefab(SerializationNode& node, std::map<GUID, GUID>& remap)
	{
		// Read the managed type and create an object based on the type
		node.ReadData("m_ScriptID", m_ScriptID);

		ScriptingManager::AddEntityScript(m_GameObject.GetGUID(), m_ScriptID);

		SerializationNode fieldsNode = node.GetNode("Fields");
		assert(fieldsNode.IsSequence());

		if (fieldsNode.HasChildren())
		{
			for (size_t i = 0; i < fieldsNode.ChildCount(); i++)
			{
				SerializationNode fieldNode = fieldsNode.GetChild(i);
				assert(fieldNode.IsMap());

				std::string fieldName;
				fieldNode.ReadData("Name", fieldName);

				// Get the fields from that type
				auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

				// Make sure the field still exists on deserialization
				for (auto& [fieldID, fieldStorage] : storage.Fields)
				{
					if (fieldStorage.Name == fieldName)
					{
						if (fieldStorage.DataType == DataType::String)
						{
							DeserializeNativeString(fieldNode, fieldStorage);
						}
						else
						{
							DeserializeNativeType(fieldNode, fieldStorage);

							// Important: We remap any field storage that contains the prefab's guids to the new instance guids
							if (fieldStorage.DataType == DataType::Entity ||
								fieldStorage.DataType == DataType::Component ||
								fieldStorage.DataType == DataType::Mesh ||
								fieldStorage.DataType == DataType::Material ||
								fieldStorage.DataType == DataType::Texture2D)
							{
								GUID value = fieldStorage.GetValue<GUID>();
								if (remap.contains(value))
								{
									value = remap[value];
									fieldStorage.SetValue<GUID>(value);
								}
							}
						}
					}
				}
			}
		}
	}

	void ScriptComponent::Deserialize(SerializationNode& node)
	{
		// Read the managed type and create an object based on the type
		node.ReadData("m_ScriptID", m_ScriptID);

		ScriptingManager::AddEntityScript(m_GameObject.GetGUID(), m_ScriptID);

		SerializationNode fieldsNode = node.GetNode("Fields");
		assert(fieldsNode.IsSequence());

		if (fieldsNode.HasChildren())
		{
			for (size_t i = 0; i < fieldsNode.ChildCount(); i++)
			{
				SerializationNode fieldNode = fieldsNode.GetChild(i);
				assert(fieldNode.IsMap());

				std::string fieldName;
				fieldNode.ReadData("Name", fieldName);

				// Get the fields from that type
				auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

				// Make sure the field still exists on deserialization
				for (auto& [fieldID, fieldStorage] : storage.Fields)
				{
					if (fieldStorage.Name == fieldName)
					{
						if (fieldStorage.DataType == DataType::String)
							DeserializeNativeString(fieldNode, fieldStorage);
						else
							DeserializeNativeType(fieldNode, fieldStorage);

					}
				}
			}
		}
	}

	void ScriptComponent::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	void ScriptComponent::SetScriptID(uint32_t scriptID)
	{
		// Remove the script from the entity storage
		ScriptingManager::RemoveEntityScript(m_GameObject.GetGUID(), m_ScriptID);

		// Update the ID
		m_ScriptID = scriptID;

		// Add the new script ID to the entity storage
		ScriptingManager::AddEntityScript(m_GameObject.GetGUID(), m_ScriptID);
	}

	void ScriptComponent::SerializeNativeTypes(SerializationNode& node, FieldStorage& storage)
	{
		SerializationNode fieldNode = node.AppendChild();
		fieldNode.SetMap();
		fieldNode.WriteData("Name", storage.Name);

		switch (storage.DataType)
		{
			case DataType::Byte:
			{
				uint8_t value = storage.GetValue<uint8_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::UShort:
			{
				uint16_t value = storage.GetValue<uint16_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::UInt:
			{
				uint32_t value = storage.GetValue<uint32_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::ULong:
			{
				uint64_t value = storage.GetValue<uint64_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::SByte:
			{
				char8_t value = storage.GetValue<char8_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Short:
			{
				int16_t value = storage.GetValue<int16_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Int:
			{
				int32_t value = storage.GetValue<int32_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Long:
			{
				int64_t value = storage.GetValue<int64_t>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Float:
			{
				float value = storage.GetValue<float>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Double:
			{
				double value = storage.GetValue<double>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Bool:
			{
				Coral::Bool32 value = storage.GetValue<Coral::Bool32>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Vector3:
			{
				glm::vec3 value = storage.GetValue<glm::vec3>();
				fieldNode.WriteData("Value", value);
				break;
			}
			case DataType::Entity:
			case DataType::Component:
			case DataType::Prefab:
			case DataType::Mesh:
			case DataType::Material:
			case DataType::Texture2D:
			{
				GUID value = storage.GetValue<GUID>();
				fieldNode.WriteData("Value", value.CRef());
				break;
			}
		}
	}

	bool ScriptComponent::SerializeNativeString(SerializationNode& node, FieldStorage& storage)
	{
		std::string fieldValue = "";

		//storedValue.resize(storage.ValueBuffer.GetSize());
		//if (storage.TryGetValue<std::string>(storedValue))
		//	fieldValue = storedValue;

		SerializationNode fieldNode = node.AppendChild();
		fieldNode.SetMap();
		fieldNode.WriteData("Name", storage.Name);
		fieldNode.WriteData("Value", fieldValue);
		return true;
	}

	template<typename T>
	inline void DeserializeValue(SerializationNode& node, FieldStorage& storage)
	{
		T value;
		node.ReadData("Value", value);
		storage.SetValue<T>(value);
	}

	void ScriptComponent::DeserializeNativeType(SerializationNode& node, FieldStorage& storage)
	{
		switch (storage.DataType)
		{
			case DataType::Byte:
			{
				DeserializeValue<uint8_t>(node, storage);
				break;
			}
			case DataType::UShort:
			{
				DeserializeValue<uint16_t>(node, storage);
				break;
			}
			case DataType::UInt:
			{
				DeserializeValue<uint32_t>(node, storage);
				break;
			}
			case DataType::ULong:
			{
				DeserializeValue<uint64_t>(node, storage);
				break;
			}
			case DataType::SByte:
			{
				DeserializeValue<char8_t>(node, storage);
				break;
			}
			case DataType::Short:
			{
				DeserializeValue<int16_t>(node, storage);
				break;
			}
			case DataType::Int:
			{
				DeserializeValue<int32_t>(node, storage);
				break;
			}
			case DataType::Long:
			{
				DeserializeValue<int64_t>(node, storage);
				break;
			}
			case DataType::Float:
			{
				DeserializeValue<float>(node, storage);
				break;
			}
			case DataType::Double:
			{
				DeserializeValue<double>(node, storage);
				break;
			}
			case DataType::Bool:
			{
				DeserializeValue<Coral::Bool32>(node, storage);
				break;
			}
			case DataType::Vector3:
			{
				DeserializeValue<glm::vec3>(node, storage);
				break;
			}
			case DataType::Entity:
			case DataType::Component:
			case DataType::Prefab:
			case DataType::Mesh:
			case DataType::Material:
			case DataType::Texture2D:
			{
				GUID value;
				node.ReadData("Value", value.Ref());
				storage.SetValue(value);
				break;
			}
		}
	}

	bool ScriptComponent::DeserializeNativeString(SerializationNode& node, FieldStorage& storage)
	{
		std::string nodeValue;
		node.ReadData("Value", nodeValue);

		//storage.ValueBuffer.Allocate(nodeValue.size());
		//storage.SetValue(nodeValue);

		return true;
	}
}