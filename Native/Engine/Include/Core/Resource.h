#pragma once

namespace Odyssey
{
	class Resource
	{
	public:
		uint32_t GetID() { return m_ID; }
		void SetID(uint32_t id) { m_ID = id; }

	protected:
		uint32_t m_ID;
	};
}