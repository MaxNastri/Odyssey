#include "ScriptComponent.h"
#include "ScriptingManager.h"
#include "String.hpp"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, ScriptComponent);

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
		managedInstance.InvokeMethod("Void Awake()");
	}

	void ScriptComponent::Update()
	{
		managedInstance.InvokeMethod("Void Update()");
	}

	void ScriptComponent::OnDestroy()
	{
		managedInstance.InvokeMethod("Void OnDestroy()");
	}

	void ScriptComponent::Serialize(SerializationNode& node)
	{
		// Get the fields from that type
		auto& storage = ScriptingManager::GetScriptStorage(m_GameObject.GetGUID());

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("m_FileID", m_FileID.CRef());
		componentNode.WriteData("m_ScriptID", m_ScriptID);
		componentNode.WriteData("Type", ScriptComponent::Type);

		SerializationNode fieldsNode = componentNode.CreateSequenceNode("Fields");

		// Make sure the field still exists on deserialization
		for (auto [fieldID, fieldStorage] : storage.Fields)
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

	void ScriptComponent::Deserialize(SerializationNode& node)
	{
		// Read the managed type and create an object based on the type
		node.ReadData("m_FileID", m_FileID.Ref());
		node.ReadData("m_ScriptID", m_ScriptID);
		node.ReadData("Name", m_ManagedType);

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
				for (auto [fieldID, fieldStorage] : storage.Fields)
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

	void ScriptComponent::SerializeNativeTypes(SerializationNode& node, FieldStorage& storage)
	{
		SerializationNode fieldNode = node.AppendChild();
		fieldNode.SetMap();
		fieldNode.WriteData("Name", storage.Name);

		switch (storage.DataType)
		{
			case DataType::Byte:
			{
				uint8_t val = storage.GetValue<uint8_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::UShort:
			{
				uint16_t val = storage.GetValue<uint16_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::UInt:
			{
				uint32_t val = storage.GetValue<uint32_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::ULong:
			{
				uint64_t val = storage.GetValue<uint64_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::SByte:
			{
				char8_t val = storage.GetValue<char8_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Short:
			{
				int16_t val = storage.GetValue<int16_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Int:
			{
				int32_t val = storage.GetValue<int32_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Long:
			{
				int64_t val = storage.GetValue<int64_t>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Float:
			{
				float val = storage.GetValue<float>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Double:
			{
				double val = storage.GetValue<double>();
				fieldNode.WriteData("Value", val);
				break;
			}
			case DataType::Bool:
			{
				Coral::Bool32 val = storage.GetValue<Coral::Bool32>();
				fieldNode.WriteData("Value", val);
				break;
			}
		}
	}

	bool ScriptComponent::SerializeNativeString(SerializationNode& node, FieldStorage& storage)
	{
		std::string fieldValue = "";
		Coral::String storedValue;

		if (storage.TryGetValue<Coral::String>(storedValue))
			fieldValue = storedValue;

		SerializationNode fieldNode = node.AppendChild();
		fieldNode.SetMap();
		fieldNode.WriteData("Name", storage.Name);
		fieldNode.WriteData("Value", fieldValue);
		return true;
	}

	template<typename T>
	inline void DeserializeValue(SerializationNode& node, FieldStorage& storage)
	{
		T val;
		node.ReadData("Value", val);
		storage.SetValue<T>(val);
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
		}
	}

	bool ScriptComponent::DeserializeNativeString(SerializationNode& node, FieldStorage& storage)
	{
		std::string nodeValue;
		node.ReadData("Value", nodeValue);

		storage.ValueBuffer.Allocate(nodeValue.size());
		storage.SetValue(nodeValue);

		return true;
	}
}