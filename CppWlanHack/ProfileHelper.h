#pragma once
#include <string>

class ProfileHelper
{
	const std::string profile_file_name = "profile.xml";
public:
	std::string get_profile_xml(const std::string &profile_name, const std::string &authentication, const std::string &encryption, const std::string &key);

	ProfileHelper();
	~ProfileHelper();
};

