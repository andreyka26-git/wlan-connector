#pragma once
#include <windows.h>

class WlanApiErrorWrapper
{
public:
	//Wraps OpenHandle method errors
	static bool wrap_open_handle_result(const DWORD);

	//Wraps EnumInterfaces method errors
	static bool wrap_enum_interfaces_result(const DWORD);

	//Wraps GetNetworkListResul method errors
	static bool wrap_get_network_list_result(const DWORD);

	//Wraps SetProfile method errors
	static bool wrap_set_profile_result(const DWORD, const DWORD);

	//Wraps Connect method errors
	static bool wrap_connect_result(DWORD);

	//Wraps WlanRegisterNotification result
	static bool wrap_set_connect_notification_result(const DWORD);
};

