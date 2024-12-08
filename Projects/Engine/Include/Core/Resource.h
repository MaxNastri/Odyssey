#pragma once

namespace Odyssey
{
	struct ResourceID
	{
	public:
		ResourceID() : m_Data(INVALID) { }
		ResourceID(uint64_t id) : m_Data(id) { }
		ResourceID(const ResourceID& other) : m_Data(other.m_Data) { }

	public:
		operator uint64_t() { return m_Data; }
		operator const uint64_t() const { return m_Data; }

	public:
		bool IsValid() { return m_Data != INVALID; }
		void Reset() { m_Data = INVALID; }
		static ResourceID Invalid() { return ResourceID(); }

	private:
		uint64_t m_Data;
		static constexpr uint64_t INVALID = (uint64_t)(-1);
	};

	class Resource
	{
	public:
		Resource() = default;
		Resource(uint64_t id) : m_ResourceID(id) { }

	public:
		ResourceID GetResourceID() { return m_ResourceID; }
		virtual void Destroy() { m_ResourceID.Reset(); }

	protected:
		friend class ResourceList;
		ResourceID m_ResourceID;
	};
}