#include "UserScript.h"
#include <Type.hpp>
#include <ScriptingManager.h>
#include <vector>

namespace Odyssey::Entities
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

	void UserScript::Serialize(json& jsonObject)
	{
		json componentJson;
		to_json(componentJson, *this);

		// Inject the managed type name
		const Coral::Type& type = managedInstance.GetType();
		std::string fqManagedName = type.GetFullName();
		componentJson += { "ManagedType", fqManagedName };

		std::vector<Coral::FieldInfo> fields = type.GetFields();

		for (auto& field : fields)
		{
			std::string fieldName = field.GetName();
			Coral::ManagedType managedType = field.GetType().GetManagedType();
			SerializeNativeTypes(managedType, fieldName, componentJson);
		}

		jsonObject += { "Component." + UserScript::ClassName, componentJson};
	}

	void UserScript::Deserialize(const json& jsonObject)
	{
		from_json(jsonObject, *this);

		// Get the managed object of this type 
		std::string managedType = jsonObject.at("ManagedType");
		managedInstance = Scripting::ScriptingManager::CreateManagedObject(managedType);

		for (auto& [key, val] : jsonObject.items())
		{
			if (val.is_object())
			{
				Coral::ManagedType managedType = val.at("ManagedType");

				std::vector<Coral::FieldInfo> fields = managedInstance.GetType().GetFields();
				for (int i = 0; i < fields.size(); ++i)
				{
					if (fields[i].GetName() == key)
					{
						DeserializeNativeType(managedType, key, val);
					}
				}
			}
		}
	}

	void UserScript::SetManagedInstance(Coral::ManagedObject instance)
	{
		managedInstance = instance;
	}

	bool UserScript::SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, json& jsonObject)
	{
		json fieldJson
		{
			{ "ManagedType", managedType }
		};

		bool nativeTypeFound = false;

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val = managedInstance.GetFieldValue<uint8_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val = managedInstance.GetFieldValue<uint16_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val = managedInstance.GetFieldValue<uint32_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val = managedInstance.GetFieldValue<uint64_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val = managedInstance.GetFieldValue<char8_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			uint16_t val = managedInstance.GetFieldValue<uint16_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			uint32_t val = managedInstance.GetFieldValue<uint32_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			uint64_t val = managedInstance.GetFieldValue<uint64_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val = managedInstance.GetFieldValue<float>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val = managedInstance.GetFieldValue<double>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			// NOTE: We case to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			bool val = managedInstance.GetFieldValue<int32_t>(fieldName);
			fieldJson.push_back({ "Value", val });
			nativeTypeFound = true;
		}

		if (nativeTypeFound)
		{
			jsonObject += {fieldName, fieldJson};
		}

		return nativeTypeFound;
	}
	bool UserScript::DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, const json& jsonObject)
	{
		bool nativeTypeFound = false;

		if (managedType == Coral::ManagedType::Byte)
		{
			uint8_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UShort)
		{
			uint16_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::UInt)
		{
			uint32_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::ULong)
		{
			uint64_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::SByte)
		{
			char8_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<char8_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Short)
		{
			uint16_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint16_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Int)
		{
			uint32_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Long)
		{
			uint64_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<uint64_t>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Float)
		{
			float val = jsonObject.at("Value");
			managedInstance.SetFieldValue<float>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Double)
		{
			double val = jsonObject.at("Value");
			managedInstance.SetFieldValue<double>(fieldName, val);
			nativeTypeFound = true;
		}
		else if (managedType == Coral::ManagedType::Bool)
		{
			// NOTE: We case to an int32_t (4 bytes) because bools in c# are treated as 4 bytes instead of 1.
			int32_t val = jsonObject.at("Value");
			managedInstance.SetFieldValue<int32_t>(fieldName, val);
			nativeTypeFound = true;
		}
		return nativeTypeFound;
	}
}