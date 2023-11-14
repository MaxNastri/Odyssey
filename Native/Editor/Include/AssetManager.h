#pragma once
#include "uuid_v4.h"

namespace Odyssey::Editor
{
	class AssetManager
	{
	public:
		std::string GenerateUUID()
		{
			return uuidGenerator.getUUID().bytes();
		}

	private:
		inline static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	};
}