#include "HackFileManager.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>

void HackFileManager::initializeEnvironment()
{
	boost::filesystem::create_directory(dictionary_folder_path);
}

void HackFileManager::savePassword(const std::string & ssid, const std::string & pass)
{
	std::ofstream file;

	file.open(file_path, std::ios::out | std::ios::app);

	if (file.is_open()) 
		file << ssid << " - " << pass << " - " <<  std::time(0) << std::endl;

	file.close();
}

std::vector<std::string>* HackFileManager::getDictionaryFilePaths()
{
	const auto dictionaries = new std::vector<std::string>();

	for (auto p : boost::filesystem::directory_iterator(dictionary_folder_path)) {
		const auto path = p.path();
		std::cout << path << std::endl;
		dictionaries->push_back(path.string());
	}

	return dictionaries;
}

std::vector<std::string>* HackFileManager::getWordsFromDictionary(const std::string & dictionary_path)
{
	std::ifstream file(dictionary_path);

	if (!file.is_open()) {
		std::cout << "Cannot read passwords from " << dictionary_path << std::endl;
		return new std::vector<std::string>();
	}

	const auto passwords = new std::vector<std::string>();

	std::string pass;
	while (std::getline(file, pass)) {
		passwords->push_back(pass);
	}

	file.close();
	return passwords;
}

HackFileManager::HackFileManager()
{
	initializeEnvironment();
}


HackFileManager::~HackFileManager()
{
}
