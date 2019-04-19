#pragma once
#include <string>
#include <wtypes.h>

class StringHelper
{
public:
	//Replace 'search' in 'subject' with 'replace'
	//P.S. spizdgeno from StackOverflow
	static void replace_string(std::string& subject, const std::string& search, const std::string& replace);
	
	//Convertors
	static std::wstring * convert_string_to_w_string(const std::string &entry);
	static char * convert_u_char_to_char(UCHAR * entry);

	//Comparers
	static bool is_equal(UCHAR * first_entry, const char* second_entry);
};

