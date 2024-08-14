#pragma once

namespace Odyssey
{
	class Resource
	{
	public:
		uint64_t GetID() { return m_ID; }
		void SetID(uint64_t id) { m_ID = id; }

	protected:
		uint64_t m_ID;
	};
}