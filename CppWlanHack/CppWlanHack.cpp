#include <iostream>
#include "WlanApiWrapper.h"

#include "HackFileManager.h"
#include <boost/filesystem.hpp>

int main()
{
	HackFileManager fileManager;

	auto dictionaries = fileManager.getDictionaryFilePaths();
	auto firstPath = dictionaries->front();

	fileManager.decompositeDictionary(firstPath);

	/*WlanApiWrapper wlanWrapper;
	wlanWrapper.connect("UKrtelecom_5E6B80");*/
	std::system("pause");
}