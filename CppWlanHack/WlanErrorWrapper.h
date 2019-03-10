#pragma once
#include <windows.h>
#include <wlanapi.h>

class WlanErrorWrapper
{
public:
	static bool wrapOpenHandleResult(const DWORD);
	static bool wrapEnumInterfacesResult(const DWORD);
	static bool wrapGetNetworkListResult(const DWORD);
	static bool wrapSetProfileResult(const DWORD, const DWORD);
	static bool wrapConnectResult(DWORD);

	WlanErrorWrapper();
	~WlanErrorWrapper();
};

