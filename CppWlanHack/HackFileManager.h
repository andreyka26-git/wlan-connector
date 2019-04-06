#pragma once
#include <string>
#include <vector>

class HackFileManager
{
	const std::string file_path = "hacked_entries.txt";
	const std::string dictionary_folder_path = "dictionary_files";
	const int count_passwords_per_file = 100000;

	void initializeEnvironment();

	//Checks is directory or file exists by path
	bool isElementExists(const std::string &path);

	//Creates directory
	void createDirectory(const std::string &path);

	//Get file path without extension
	std::string * getFilePathWithoutExtension(const std::string &path);

	//Build path according to given directory, name of file and extension of file. 
	//If it has absolute path, it should be placed with 'dir' parameter
	std::string * buildPath(const std::string &dir, const std::string &name_file, const std::string &extension);

public:
	//Saves password to file with appropriate ssid
	void savePassword(const std::string &ssid, const std::string &pass);
	
	//Gets all files, that exists in diictionary_files directory
	std::vector<std::string> * getDictionaryFilePaths();

	//Divede massive file with passwords into many smaller, in order to skip it in future.
	void decompositeDictionary(const std::string &);

	HackFileManager();
	~HackFileManager();
};

