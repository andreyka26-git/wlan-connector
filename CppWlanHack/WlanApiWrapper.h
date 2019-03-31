#pragma once
#include <windows.h>
#include <wlanapi.h>
#include <vector>

#include "ProfileHelper.h"
#include "WlanApiErrorWrapper.h"
#include "PasswordManager.h"

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

	//Password manager, saves and make operations on ssids and passwords.
	PasswordManager * pass_manager = nullptr;

	//METHODS
	//Try to scan network and exit app if not entries is found.
	void scanEntries();

	//Initialize wlan handle to work with Wlan
	bool trySetWlanClient();

	//Initialize wlan interface (wifi driver) to work with Wlan
	bool trySetWlanInterfaceInfo();

	//Gets all open wifi endpoints
	PWLAN_AVAILABLE_NETWORK_LIST getAvailableEntries();

	//Connect via RSNAPSK algorithm
	std::string * connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry);
public:
	//Try to connects to all wifi
	void connectToAll();

	//Try connect to specefic SSID
	void connect(std::string ssid);

	//Scans and saves wifi entries to memory
	void ensureEntries();

	WlanApiWrapper();
	~WlanApiWrapper();
};

