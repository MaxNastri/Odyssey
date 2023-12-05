#pragma once

namespace Odyssey
{
	class Asset
	{
	public:
		void SetUUID(const std::string& uuid) { m_UUID = uuid; }
		void SetPath(const std::string& path) { m_AssetPath = path; }
		void SetType(const std::string& type) { m_Type = type; }

	public:
		const std::string& GetUUID() { return m_UUID; }

	protected:
		std::string m_UUID;
		std::string m_Name;
		std::string m_AssetPath;
		std::string m_Type;
	};
}