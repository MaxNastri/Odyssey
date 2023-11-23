#pragma once

namespace Odyssey
{
	class Resource
	{
	public:
		uint32_t GetID() { return m_ID; }

	protected:
		friend class ResourceManager;
		uint32_t m_ID;
	};
}