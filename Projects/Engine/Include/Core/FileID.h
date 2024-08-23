#pragma once

namespace Odyssey
{
	class FileID
	{
	public:
		FileID();
		FileID(uint32_t fileID);
		FileID(const std::string& fileID);
		FileID(const FileID& other);

	public:
		operator uint32_t() { return m_FileID; }
		operator const uint32_t() const { return m_FileID; }

	public:
		static FileID Empty() { return FileID(0); }
		uint32_t& Ref() { return m_FileID; }
		const uint32_t& CRef() { return m_FileID; }
		std::string String() { return std::to_string(m_FileID); }
	private:
		uint32_t m_FileID;
	};
}