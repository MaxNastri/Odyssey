#pragma once
#include "ManagedObject.hpp"
#include "GUID.h"
#include "FileID.h"
#include "RawBuffer.hpp"
#include "DataType.h"
#include "GUID.h"

namespace Odyssey
{
	struct FieldStorage
	{
		std::string Name;
		Coral::Type* Type;
		DataType DataType;
		RawBuffer ValueBuffer;
		Coral::ManagedObject* Instance;

		template<typename T>
		T GetValue() const
		{
			return Instance ? Instance->GetFieldValue<T>(Name) : ValueBuffer.Read<T>();
		}

		template<typename T>
		bool TryGetValue(T& outValue)
		{
			if (Instance)
			{
				return true;
			}
			else
			{
				if (ValueBuffer.GetSize() == 0)
					return false;
				
				outValue = ValueBuffer.Read<T>();
				return true;
			}
		}

		template<typename T>
		void SetValue(const T& value)
		{
			if (Instance)
				Instance->SetFieldValue<T>(Name, value);
			else
				ValueBuffer.Write(&value, sizeof(T));
		}
	};

	struct ScriptStorage
	{
		uint32_t ScriptID;
		std::map<uint32_t, FieldStorage> Fields;
		Coral::ManagedObject* Instance;
	};
}