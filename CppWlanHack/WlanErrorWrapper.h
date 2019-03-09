#pragma once
#include <windows.h>
class WlanErrorWrapper
{
public:
	static bool WrapOpenHandleResult(DWORD);

	WlanErrorWrapper();
	~WlanErrorWrapper();
};

