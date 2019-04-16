#include "StringHelper.h"

void StringHelper::replace_string(std::string & subject, const std::string & search, const std::string & replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::wstring * StringHelper::convert_string_to_w_string(const std::string &entry)
{
	return new std::wstring(std::begin(entry), std::end(entry));
}

char * StringHelper::convert_u_char_to_char(UCHAR * entry)
{
	return reinterpret_cast<char*>(entry);
}

bool StringHelper::is_equal(UCHAR * first_entry, const char * second_entry)
{
	return strcmp(reinterpret_cast<char*>(first_entry), second_entry) == 0;
}