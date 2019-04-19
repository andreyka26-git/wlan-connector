#include "ProfileHelper.h"
#include <string>
#include <fstream>

#include "StringHelper.h"

std::string * ProfileHelper::get_profile_xml(const std::string & profile_name, const std::string & authentication, const std::string & encryption, const std::string & key) const
{
	std::ifstream xml(profile_file_name);
	std::string *xml_content = nullptr;

	if (xml) {
		xml_content = new std::string((std::istreambuf_iterator<char>(xml)),
			std::istreambuf_iterator<char>());

		StringHelper::replace_string(*xml_content, "<name>{0}</name>", "<name>" + profile_name + "</name>");
		StringHelper::replace_string(*xml_content, "<authentication>{1}</authentication>", "<authentication>" + authentication + "</authentication>");
		StringHelper::replace_string(*xml_content, "<encryption>{2}</encryption>", "<encryption>" + encryption + "</encryption>");
		StringHelper::replace_string(*xml_content, "<keyMaterial>{3}</keyMaterial>", "<keyMaterial>" + key + "</keyMaterial>");
	}

	xml.close();
	return xml_content;
}
