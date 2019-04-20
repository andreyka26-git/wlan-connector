#pragma once
#include <windows.h>
#include <wlanapi.h>
#include <vector>

#include "ProfileHelper.h"

class WlanApiWrapper
{
	typedef bool(WlanApiWrapper::* (connect_func) )(WLAN_AVAILABLE_NETWORK, const char *, const char *);

	//CONSTS
	const char * DefaultAuthenticationProtocol = "WPA2PSK";
	const char * DefaultEncryption = "AES";

	//FIELDS
	HANDLE wlan_client = nullptr;
	PWLAN_INTERFACE_INFO wlan_interface_info = nullptr;

	std::vector<WLAN_AVAILABLE_NETWORK> *entries = nullptr;

	//SERVICES
	//Service for creating and managing of xml profile
	ProfileHelper * profile_helper = nullptr;

	//METHODS
	//Try to scan network and exit app if not entries is found.
	void scan_entries() const;

	//Initialize wlan handle to work with Wlan
	bool try_set_wlan_client();

	//Initialize wlan interface (wifi driver) to work with Wlan
	bool try_set_wlan_interface_info();

	//Gets all open wifi endpoints
	PWLAN_AVAILABLE_NETWORK_LIST getAvailableEntries() const;

	//Connect via RSNAPSK algorithm
	bool try_connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry, const char *ssid, const char * pass);

	//MapAuthAlgorithm
	static connect_func map_algorithm_to_method(WLAN_AVAILABLE_NETWORK & network);

	//Build wlan connection parameters
	static PWLAN_CONNECTION_PARAMETERS build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, const char *ssid, const char * pass);
public:
	//Try connect to specific SSID
	bool connect(const char *ssid, const char * pass) const;

	//Scans and saves wifi entries to memory
	void ensure_entries() const;

	WlanApiWrapper();
	~WlanApiWrapper();
};

