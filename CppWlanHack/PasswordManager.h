#pragma once
#include <string>

class PasswordManager
{
	const std::string filePath = "hacked_entries.txt";
public:
	void savePassword(const std::string &ssid, const std::string &pass);

	PasswordManager();
	~PasswordManager();
};

