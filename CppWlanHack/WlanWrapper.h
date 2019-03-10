#pragma once
#include <windows.h>
#include <wlanapi.h>

#include "ProfileHelper.h"

class WlanWrapper
{
	HANDLE wlan_client = nullptr;
	PWLAN_INTERFACE_INFO wlan_interface_info = nullptr;
	ProfileHelper * profile_helper = nullptr;

	bool initializeWlanClient();
	bool initializeWlanInterfaceInfo();
	PWLAN_AVAILABLE_NETWORK_LIST getAvailableEntries();

	void connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK);
public:
	void connectToWifi();

	WlanWrapper();
	~WlanWrapper();
};

