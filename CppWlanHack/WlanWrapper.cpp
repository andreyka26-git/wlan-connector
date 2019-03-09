#include "WlanWrapper.h"
#include <wlanapi.h>
#include <iostream>

#include "WlanErrorWrapper.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

HANDLE WlanWrapper::initializeWlanClient()
{
	HANDLE h_client = nullptr;
	const DWORD dwMaxClient = 2;
	DWORD dw_cur_version = 0;

	auto openHandleResult = WlanOpenHandle(dwMaxClient, nullptr, &dw_cur_version, &h_client);
	const auto isSuccess = WlanErrorWrapper::WrapOpenHandleResult(openHandleResult);

	if (!isSuccess)
		return nullptr;

	return h_client;
}

WlanWrapper::WlanWrapper()
{
}

WlanWrapper::~WlanWrapper()
{
}
