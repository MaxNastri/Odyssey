#pragma once
#include "ManagedObject.hpp"
#include "GUID.h"
#include "FileID.h"
#include "RawBuffer.hpp"
#include "DataType.h"

namespace Odyssey
{
	struct FieldStorage
	{
		std::string Name;
		Coral::Type* Type;
		DataType DataType;
		RawBuffer ValueBuffer;
		Coral::ManagedObject* Instance;
	};

	struct ScriptStorage
	{
		uint32_t ScriptID;
		std::map<uint32_t, FieldStorage> Fields;
		Coral::ManagedObject* Instance;
	};
}