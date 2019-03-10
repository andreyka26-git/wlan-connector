#pragma once
#include <string>

class StringHelper
{
public:
	static void replaceString(std::string& subject, const std::string& search, const std::string& replace);

	StringHelper();
	~StringHelper();
};

