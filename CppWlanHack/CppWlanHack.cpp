#include <iostream>
#include "WlanApiWrapper.h"

#include <boost/filesystem.hpp>

int main()
{
	WlanApiWrapper wlanWrapper;
	wlanWrapper.connect("UKrtelecom_5E6B80");
	std::system("pause");
}