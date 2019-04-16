#include "ProfileHelper.h"
#include <string>
#include <fstream>

#include "StringHelper.h"

std::string ProfileHelper::get_profile_xml(const std::string & profile_name, const std::string & authentication, const std::string & encryption, const std::string & key)
{
	std::ifstream xml(profile_file_name);
	std::string xmlContent;

	if (xml) {
		xmlContent = std::string((std::istreambuf_iterator<char>(xml)),
			std::istreambuf_iterator<char>());

		StringHelper::replaceString(xmlContent, "<name>{0}</name>", "<name>" + profile_name + "</name>");
		StringHelper::replaceString(xmlContent, "<authentication>{1}</authentication>", "<authentication>" + authentication + "</authentication>");
		StringHelper::replaceString(xmlContent, "<encryption>{2}</encryption>", "<encryption>" + encryption + "</encryption>");
		StringHelper::replaceString(xmlContent, "<keyMaterial>{3}</keyMaterial>", "<keyMaterial>" + key + "</keyMaterial>");
	}

	xml.close();
	return xmlContent;
}

ProfileHelper::ProfileHelper()
{
}

ProfileHelper::~ProfileHelper()
{
}
