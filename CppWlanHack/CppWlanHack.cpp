#include <iostream>
#include "WlanApiWrapper.h"

int main()
{
	WlanApiWrapper wlanWrapper;
	wlanWrapper.tryConnect("UKrtelecom_5E6B80");
	std::system("pause");
}