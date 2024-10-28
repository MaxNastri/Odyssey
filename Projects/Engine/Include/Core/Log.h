#pragma once
#include <string>
#include <format>

namespace Odyssey
{
	class Log
	{
	public:
		static void Info(const char* message);
		static void Info(std::string message);
		static void Info(std::string_view message);
		static void Warning(const char* message);
		static void Warning(std::string message);
		static void Warning(std::string_view message);
		static void Error(const char* message);
		static void Error(std::string message);
		static void Error(std::string_view message);
	};
}