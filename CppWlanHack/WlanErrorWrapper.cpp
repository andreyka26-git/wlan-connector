#include "WlanErrorWrapper.h"
#include <iostream>
#include <windows.h>

bool WlanErrorWrapper::WrapOpenHandleResult(DWORD openHandleResult)
{
	if (openHandleResult != ERROR_SUCCESS) {
		std::cout << "WlanOpenHandle failed with error: " << openHandleResult << std::endl;
		return false;
	}

	return true;
}

WlanErrorWrapper::WlanErrorWrapper()
{
}

WlanErrorWrapper::~WlanErrorWrapper()
{
}
