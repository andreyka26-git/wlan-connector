#include "StringHelper.h"

void StringHelper::replaceString(std::string & subject, const std::string & search, const std::string & replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::wstring * StringHelper::convertStringToWString(const std::string &stringToConvert)
{
	return new std::wstring(std::begin(stringToConvert), std::end(stringToConvert));
}

StringHelper::StringHelper()
{
}

StringHelper::~StringHelper()
{
}
