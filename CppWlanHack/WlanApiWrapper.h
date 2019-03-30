#pragma once
#include <windows.h>
#include <wlanapi.h>
#include <vector>

#include "ProfileHelper.h"
#include "WlanApiErrorWrapper.h"

class WlanApiWrapper
{
	//FIELDS
	HANDLE wlan_client = nullptr;
	PWLAN_INTERFACE_INFO wlan_interface_info = nullptr;

	std::vector<WLAN_AVAILABLE_NETWORK> *entries = nullptr;

	//SERVICES
	//Service for creating and managing of xml profile
	ProfileHelper * profile_helper = nullptr;

	//Service for wrapping method result of Wlan api
	WlanApiErrorWrapper * error_wrapper = nullptr;

	//METHODS
	//Try to scan network and exit app if not entries is found.
	void tryScanEntries();

	//Initialize wlan handle to work with Wlan
	bool initializeWlanClient();

	//Initialize wlan interface (wifi driver) to work with Wlan
	bool initializeWlanInterfaceInfo();

	//Gets all open wifi endpoints
	PWLAN_AVAILABLE_NETWORK_LIST getAvailableEntries();

	//Connect via RSNAPSK algorithm
	std::string connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry);
public:
	//Try to connects to all wifi
	void tryConnectToAll();

	//Try connect to specefic SSID
	void tryConnect(std::string ssid);

	//Scans and saves wifi entries to memory
	void scanEntries();

	WlanApiWrapper();
	~WlanApiWrapper();
};

