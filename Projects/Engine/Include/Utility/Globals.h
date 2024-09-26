#pragma once
#include <string>

namespace Odyssey
{
	// Enums
	namespace Globals
	{
#pragma region Macros

#define TO_STRING( x ) #x
#define CLASS_DECLARATION( classNamespace, className ) \
public: \
	inline static std::string ClassNamespace = TO_STRING(classNamespace); \
	inline static std::string ClassName = TO_STRING(className); \
	inline static std::string Type = TO_STRING(classNamespace.className);

#pragma endregion

		inline static void ReplaceString(std::string& str, const char* token, std::string_view value)
		{
			size_t pos = 0;
			while ((pos = str.find(token, pos)) != std::string::npos)
			{
				str.replace(pos, strlen(token), value);
				pos += strlen(token);
			}
		}

		inline static std::filesystem::path GetApplicationPath()
		{
			wchar_t buffer[MAX_PATH];
			GetModuleFileName(NULL, buffer, sizeof(buffer));
			std::filesystem::path outPath = std::filesystem::path(buffer).parent_path();
			return outPath;
		}
	}

}