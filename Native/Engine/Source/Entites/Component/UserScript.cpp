#include "UserScript.h"
#include <Type.hpp>
#include <ScriptingManager.h>
#include <vector>
#include <String.hpp>

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey.Entities, UserScript);

	UserScript::UserScript(const std::string& managedType)
	{
		managedInstance = Scripting::ScriptingManager::CreateManagedObject(managedType);
	}

	void UserScript::Awake()
	{
		managedInstance.InvokeMethod("Void Awake()");
	}

	void UserScript::Update()
	{
		managedInstance.InvokeMethod("Void Update()");
	}

	void UserScript::OnDestroy()
	{
		managedInstance.InvokeMethod("Void OnDestroy()");
	}

	void UserScript::Serialize(ryml::NodeRef& node)
	{
		const Coral::Type& type = managedInstance.GetType();
		std::string fqManagedName = type.GetFullName();

		ryml::NodeRef componentNode = node.append_child();
		componentNode |= ryml::MAP;

		componentNode["Name"] << fqManagedName;

		std::vector<Coral::FieldInfo> fields = type.GetFields();

		ryml::NodeRef fieldsNode = componentNode["Fields"];
		fieldsNode = ryml::SEQ;
		for (auto& field : fields)
		{
			std::string fieldName = field.GetName();
			Coral::Type fieldType = field.GetType();

			if (fieldType.IsString())
			{
				SerializeNativeString(fieldName, fieldsNode);
			}
			else
			{
				Coral::ManagedType managedType = fieldType.GetManagedType();
				if (managedType != Coral::ManagedType::Unknown)
					SerializeNativeTypes(managedType, fieldName, fieldsNode);
			}
		}
	}

	void UserScript::Deserialize(ryml::ConstNodeRef node)
	{
		const Coral::Type& type = managedInstance.GetType();
		std::vector<Coral::FieldInfo> fields = type.GetFields();

		ryml::ConstNodeRef fieldsNode = node["Fields"];
		assert(fieldsNode.is_seq());

		if (fieldsNode.has_children())
		{
			for (size_t i = 0; i < fieldsNode.num_children(); i++)
			{
				ryml::ConstNodeRef fieldNode = fieldsNode.child(i);
				assert(fieldNode.is_map());

				std::string fieldName;
				std::string managedType;
				fieldNode["Name"] >> fieldName;
				fieldNode["ManagedType"] >> managedType;

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
							fieldNode["ManagedType"] >> managedTypeInt;
							DeserializeNativeType((Coral::ManagedType)managedTypeInt, fieldName, fieldNode);
						}
					}
				}
			}
		}
	}

	void UserScript::SetManagedInstance(Coral::ManagedObject instance)
	{
		managedInstance = instance;
	}

	void UserScript::SetManagedType(std::string_view managedClassName)
	{
		// TODO (MAX): Destroy any existing managed instance first
		managedInstance = Scripting::ScriptingManager::CreateManagedObject(managedClassName);
	}

	bool UserScript::SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::NodeRef node)
	{
		bool nativeTypeFound = false;

		ryml::NodeRef fieldNode = node.append_child();
		fieldNode |= ryml::MAP;
		fieldNode["Name"] << fieldName;
		fieldNode["ManagedType"] << (uint32_t)managedType;

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val = managedInstance.GetFieldValue<uint8_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val = managedInstance.GetFieldValue<uint16_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val = managedInstance.GetFieldValue<uint32_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val = managedInstance.GetFieldValue<uint64_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val = managedInstance.GetFieldValue<char8_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			int16_t val = managedInstance.GetFieldValue<int16_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			int32_t val = managedInstance.GetFieldValue<int32_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			int64_t val = managedInstance.GetFieldValue<int64_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val = managedInstance.GetFieldValue<float>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val = managedInstance.GetFieldValue<double>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			// NOTE: We case to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			bool val = managedInstance.GetFieldValue<int32_t>(fieldName);
			fieldNode["Value"] << val;
			nativeTypeFound = true;
		}

		return nativeTypeFound;
	}

	bool UserScript::SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node)
	{
		Coral::String fieldValue = managedInstance.GetFieldValue<Coral::String>(fieldName);

		ryml::NodeRef fieldNode = node.append_child();
		fieldNode |= ryml::MAP;
		fieldNode["Name"] << fieldName;
		fieldNode["ManagedType"] << "String";
		fieldNode["Value"] << std::string(fieldValue);
		return true;
	}

	bool UserScript::DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::ConstNodeRef node)
	{
		bool nativeTypeFound = false;

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<uint8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<uint16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<uint32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<uint64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<char8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			int16_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<int16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			int32_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<int32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			int64_t val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<int64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<float>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val;
			node["Value"] >> val;
			managedInstance.SetFieldValue<double>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			bool val;
			node["Value"] >> val;
			// NOTE: We case to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			managedInstance.SetFieldValue<int32_t>(fieldName, val);
			nativeTypeFound = true;
		}

		return nativeTypeFound;
	}

	bool UserScript::DeserializeNativeString(const std::string& fieldName, ryml::ConstNodeRef node)
	{
		std::string nodeValue;
		node["Value"] >> nodeValue;
		Coral::ScopedString fieldValue = Coral::String::New(nodeValue);
		managedInstance.SetFieldValue<Coral::String>(fieldName, fieldValue);
		return true;
	}
}