#pragma once

namespace Odyssey::Framework
{
	class Log
	{
	public:
		static void Info(const char* message);
		static void Warning(const char* message);
		static void Error(const char* message);
		static void Error(std::string_view message);
	};
}