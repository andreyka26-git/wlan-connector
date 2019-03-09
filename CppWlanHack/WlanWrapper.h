#pragma once
#include <windows.h>

class WlanWrapper
{
public:
	HANDLE initializeWlanClient();

	WlanWrapper();
	~WlanWrapper();
};

