#pragma once

namespace Odyssey
{
	class Asset
	{
	public:
		void SetGUID(const std::string& guid) { m_GUID = guid; }
		void SetPath(const std::string& path) { m_AssetPath = path; }
		void SetType(const std::string& type) { m_Type = type; }

	public:
		const std::string& GetGUID() { return m_GUID; }

	protected:
		std::string m_GUID;
		std::string m_Name;
		std::string m_AssetPath;
		std::string m_Type;
	};
}