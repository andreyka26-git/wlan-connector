#pragma once
#include <string>

class ProfileHelper
{
	const std::string profile_file_name = "profile.xml";
public:
	//Changes xml profile template with appropriate parameteres to set the WLAN profile.
	std::string * get_profile_xml(const std::string &profile_name, const std::string &authentication, const std::string &encryption, const std::string &key) const;
};

