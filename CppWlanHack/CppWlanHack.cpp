#include <iostream>
#include "WlanWrapper.h"

int main()
{
	WlanWrapper wlanWrapper;
	wlanWrapper.connectToWifi();
	std::system("pause");
}