#pragma once
#include <string>

namespace Odyssey
{
	namespace Globals
	{
#pragma region Macros

#define TO_STRING( x ) #x
#define CLASS_DECLARATION( classname ) \
public: \
	static std::string ClassNamespace; \
	static std::string ClassName; \
	static std::string Type;
#define CLASS_DEFINITION( classNamespace, className ) \
std::string className::ClassNamespace = TO_STRING( classNamespace ); \
std::string className::ClassName = TO_STRING( className ); \
std::string className::Type = TO_STRING(classNamespace.className);

#pragma endregion

		inline static void ReplaceString(std::string& str, const char* token, const std::string& value)
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