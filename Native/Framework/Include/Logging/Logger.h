#pragma once
#include <string>
#include <format>

namespace Odyssey::Framework
{
	class Logger
	{
	public:
		static void LogInfo(const char* message);
		static void LogInfo(std::string message);
		static void LogInfo(std::string_view message);
		static void LogWarning(const char* message);
		static void LogError(const char* message);
		static void LogError(std::string message);
		static void LogError(std::string_view message);
	};
}