#include "PasswordManager.h"
#include <fstream>
#include <ctime>

void PasswordManager::savePassword(const std::string & ssid, const std::string & pass)
{
	std::ofstream file;

	file.open(filePath, std::ios::out | std::ios::app);

	if (file.is_open()) 
		file << ssid << " - " << pass << std::time(0) << std::endl;

	file.close();
}

PasswordManager::PasswordManager()
{
}


PasswordManager::~PasswordManager()
{
}
