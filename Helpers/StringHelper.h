#pragma once
#include <string>

class StringHelper
{
public:
	//Replace 'search' in 'subject' with 'replace'
	//P.S. spizdgeno from StackOverflow
	static void replaceString(std::string& subject, const std::string& search, const std::string& replace);
	static std::wstring * convertStringToWString(const std::string &stringToConvert);

	StringHelper();
	~StringHelper();
};

