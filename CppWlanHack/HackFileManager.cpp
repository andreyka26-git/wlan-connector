#include "HackFileManager.h"
#include <fstream>
#include <ctime>
#include <direct.h>

void HackFileManager::initializeEnvironment()
{
	mkdir(dictionary_folder_path.c_str());
}

void HackFileManager::savePassword(const std::string & ssid, const std::string & pass)
{
	std::ofstream file;

	file.open(file_path, std::ios::out | std::ios::app);

	if (file.is_open()) 
		file << ssid << " - " << pass << " - " <<  std::time(0) << std::endl;

	file.close();
}

HackFileManager::HackFileManager()
{
	initializeEnvironment();
}


HackFileManager::~HackFileManager()
{
}
