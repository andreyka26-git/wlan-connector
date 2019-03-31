#pragma once
#include <string>
#include <vector>

class HackFileManager
{
	const std::string file_path = "hacked_entries.txt";
	const std::string dictionary_folder_path = "dictionary_files";

	void initializeEnvironment();
public:
	void savePassword(const std::string &ssid, const std::string &pass);
	std::vector<std::string> * getDictionaryFilePaths();
	std::vector<std::string> * getWordsFromDictionary(const std::string &dictionary_name);

	HackFileManager();
	~HackFileManager();
};

