#pragma once
#include <string>

#define TO_STRING( x ) #x

#define CLASS_DECLARATION( classname ) \
public: \
	static std::string ClassNamespace; \
	static std::string ClassName;

#define CLASS_DEFINITION( classNamespace, className ) \
std::string className::ClassNamespace = TO_STRING( classNamespace ); \
std::string className::ClassName = TO_STRING( className );