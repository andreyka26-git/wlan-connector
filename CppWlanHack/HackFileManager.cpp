#include "HackFileManager.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>

void HackFileManager::initializeEnvironment()
{
	boost::filesystem::create_directory(dictionary_folder_path);
}

bool HackFileManager::isElementExists(const std::string & path)
{
	boost::filesystem::path dir_path(path.c_str());
	return boost::filesystem::exists(dir_path);
}

void HackFileManager::createDirectory(const std::string & path)
{
	boost::filesystem::path dir_path(path.c_str);
	boost::filesystem::create_directory(dir_path);
}

std::string * HackFileManager::getFilePathWithoutExtension(const std::string & file_path)
{
	auto file_path_without_extension = new std::string();

	for (auto character: file_path) {
		if (character == '.')
			break;

		(*file_path_without_extension) += character;
	}
	return file_path_without_extension;
}

std::string * HackFileManager::buildPath(const std::string & dir, const std::string & name_file, const std::string & extension)
{
	if (!isElementExists(dir))
		createDirectory(dir);

	auto path = new std::string(dir + "\\" + name_file + "." + extension);
	return path;
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

		if (boost::filesystem::is_regular_file(p.path()))
			continue;

		const auto path = p.path().string();

		std::cout << path << std::endl;
		dictionaries->push_back(path);
	}

	return dictionaries;
}

void HackFileManager::decompositeDictionary(const std::string & file_passwords_path)
{
	auto file_path_without_extension = getFilePathWithoutExtension(file_passwords_path);
	const boost::filesystem::path path_to_new_directory(file_path_without_extension->c_str());

	if (!boost::filesystem::create_directory(path_to_new_directory))
	{
		std::cout << "Cannot create directory by " << path_to_new_directory << " path" << std::endl;
		return;
	}

	int file_index = 1;

	auto new_file_path = buildPath(*file_path_without_extension, std::to_string(file_index), "txt");
	std::ofstream new_passwords(*new_file_path);

	if (!new_passwords.is_open())
	{
		std::cout << "Cannot open file by " << new_file_path << " path" << std::endl;
		return;
	}

	int passwords_count = 0;

	std::ifstream old_passwords(file_passwords_path);
	std::string pass;

	while (std::getline(old_passwords, pass)) {

		if (passwords_count == count_passwords_per_file) {
			new_passwords.close();
 
			new_file_path = buildPath(*file_path_without_extension, std::to_string(file_index), "txt");;
			new_passwords = std::ofstream(*new_file_path);
			
			passwords_count = 0;
			file_index++;
		}

		new_passwords << pass << std::endl;
		passwords_count++;
	}

	new_passwords.close();
	old_passwords.close();

	delete file_path_without_extension;
	delete new_file_path;
}

HackFileManager::HackFileManager()
{
	initializeEnvironment();
}


HackFileManager::~HackFileManager()
{
}
