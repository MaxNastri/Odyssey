#pragma once

namespace Odyssey::Rune
{
	struct GlobalData
	{
		uint64_t NextID = 1;
	};

	inline extern struct GlobalData g_Data { };
	inline uint64_t GetNextID() { return g_Data.NextID++; }
}