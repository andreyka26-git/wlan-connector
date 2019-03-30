#pragma once
#include <windows.h>
#include <wlanapi.h>

class WlanApiErrorWrapper
{
public:
	//Wraps OpenHandle method errors
	bool wrapOpenHandleResult(const DWORD);

	//Wraps EnumInterfaces method errors
	bool wrapEnumInterfacesResult(const DWORD);

	//Wraps GetNetworkListResul method errors
	bool wrapGetNetworkListResult(const DWORD);

	//Wraps SetProfile method errors
	bool wrapSetProfileResult(const DWORD, const DWORD);

	//Wraps Connect method errors
	bool wrapConnectResult(DWORD);

	WlanApiErrorWrapper();
	~WlanApiErrorWrapper();
};

