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
		const Coral::Type& type = managedInstance.GetType();
		std::string fqManagedName = type.GetFullName();
		std::vector<Coral::FieldInfo> fields = type.GetFields();

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("m_FileID", m_FileID.CRef());
		componentNode.WriteData("Type", ScriptComponent::Type);
		componentNode.WriteData("Name", fqManagedName);

		SerializationNode fieldsNode = componentNode.CreateSequenceNode("Fields");
		for (auto& field : fields)
		{
			std::string fieldName = field.GetName();
			Coral::Type fieldType = field.GetType();

			//if (fieldType.IsString())
			//{
			//	SerializeNativeString(fieldName, fieldsNode);
			//}
			//else
			{
				Coral::ManagedType managedType = fieldType.GetManagedType();
				if (managedType != Coral::ManagedType::Unknown)
					SerializeNativeTypes(managedType, fieldName, fieldsNode);
			}
		}
	}

	void ScriptComponent::Deserialize(SerializationNode& node)
	{
		// Read the managed type and create an object based on the type
		node.ReadData("m_FileID", m_FileID.Ref());
		node.ReadData("Name", m_ManagedType);
		managedInstance = ScriptingManager::CreateManagedObject(m_ManagedType, m_GameObject.GetGUID());

		// Get the fields from that type
		const Coral::Type& type = managedInstance.GetType();
		std::vector<Coral::FieldInfo> fields = type.GetFields();

		SerializationNode fieldsNode = node.GetNode("Fields");
		assert(fieldsNode.IsSequence());

		if (fieldsNode.HasChildren())
		{
			for (size_t i = 0; i < fieldsNode.ChildCount(); i++)
			{
				SerializationNode fieldNode = fieldsNode.GetChild(i);
				assert(fieldNode.IsMap());

				std::string fieldName;
				std::string managedType;
				fieldNode.ReadData("Name", fieldName);
				fieldNode.ReadData("ManagedType", managedType);

				// Make sure the field still exists on deserialization
				for (auto& field : fields)
				{
					if (field.GetName() == fieldName)
					{
						if (managedType == "String")
						{
							DeserializeNativeString(fieldName, fieldNode);
						}
						else
						{
							uint32_t managedTypeInt;
							fieldNode.ReadData("ManagedType", managedTypeInt);
							DeserializeNativeType((Coral::ManagedType)managedTypeInt, fieldName, fieldNode);
						}
					}
				}
			}
		}
	}

	bool ScriptComponent::SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node)
	{
		bool nativeTypeFound = false;

		SerializationNode fieldNode = node.AppendChild();
		fieldNode.SetMap();
		fieldNode.WriteData("Name", fieldName);
		fieldNode.WriteData("ManagedType", (uint32_t)managedType);

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val = managedInstance.GetFieldValue<uint8_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val = managedInstance.GetFieldValue<uint16_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val = managedInstance.GetFieldValue<uint32_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val = managedInstance.GetFieldValue<uint64_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val = managedInstance.GetFieldValue<char8_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			int16_t val = managedInstance.GetFieldValue<int16_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			int32_t val = managedInstance.GetFieldValue<int32_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			int64_t val = managedInstance.GetFieldValue<int64_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val = managedInstance.GetFieldValue<float>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val = managedInstance.GetFieldValue<double>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			// NOTE: We cast to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			bool val = managedInstance.GetFieldValue<int32_t>(fieldName);
			fieldNode.WriteData("Value", val);
			nativeTypeFound = true;
		}

		return nativeTypeFound;
	}

	bool ScriptComponent::SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node)
	{
		Coral::String fieldValue = managedInstance.GetFieldValue<Coral::String>(fieldName);

		ryml::NodeRef fieldNode = node.append_child();
		fieldNode |= ryml::MAP;
		fieldNode["Name"] << fieldName;
		fieldNode["ManagedType"] << "String";
		fieldNode["Value"] << std::string(fieldValue);
		return true;
	}

	bool ScriptComponent::DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, SerializationNode& node)
	{
		bool nativeTypeFound = false;

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<uint8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<uint16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<uint32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<uint64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<char8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			int16_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<int16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			int32_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<int32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			int64_t val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<int64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<float>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val;
			node.ReadData("Value", val);
			managedInstance.SetFieldValue<double>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			bool val;
			node.ReadData("Value", val);
			// NOTE: We case to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			managedInstance.SetFieldValue<int32_t>(fieldName, val);
			nativeTypeFound = true;
		}

		return nativeTypeFound;
	}

	bool ScriptComponent::DeserializeNativeString(const std::string& fieldName, SerializationNode& node)
	{
		std::string nodeValue;
		node.ReadData("Value", nodeValue);

		Coral::ScopedString fieldValue = Coral::String::New(nodeValue);
		managedInstance.SetFieldValue<Coral::String>(fieldName, fieldValue);
		return true;
	}
}