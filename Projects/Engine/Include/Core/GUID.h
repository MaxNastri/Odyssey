#pragma once

namespace Odyssey
{
	class GUID
	{
	public:
		GUID();
		GUID(uint64_t guid);
		GUID(const std::string& guid);
		GUID(const GUID& other);
		static GUID New();

	public:
		operator uint64_t() { return m_GUID; }
		operator const uint64_t() const { return m_GUID; }

	public:
		static GUID Empty() { return GUID(0); }
		uint64_t& Ref() { return m_GUID; }
		const uint64_t& CRef() { return m_GUID; }
		std::string String() { return std::to_string(m_GUID); }
	private:
		uint64_t m_GUID;
	};
}