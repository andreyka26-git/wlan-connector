#include "WlanWrapper.h"
#include <wlanapi.h>
#include <iostream>
#include <wtypes.h>
#include <codecvt>

#include "WlanErrorWrapper.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

bool WlanWrapper::initializeWlanClient()
{
	const DWORD dw_max_client = 2;
	DWORD dw_cur_version = 0;

	auto open_handle_result = WlanOpenHandle(dw_max_client, nullptr, &dw_cur_version, &wlan_client);
	const auto isSuccess = WlanErrorWrapper::wrapOpenHandleResult(open_handle_result);
	
	return isSuccess;
}

bool WlanWrapper::initializeWlanInterfaceInfo()
{
	PWLAN_INTERFACE_INFO_LIST interfaces_list = nullptr;
	const auto enum_interfaces_result = WlanEnumInterfaces(wlan_client, nullptr, &interfaces_list);

	const auto isSuccess = WlanErrorWrapper::wrapEnumInterfacesResult(enum_interfaces_result);

	wlan_interface_info = interfaces_list->InterfaceInfo;
	return isSuccess;
}

PWLAN_AVAILABLE_NETWORK_LIST WlanWrapper::getAvailableEntries()
{
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;
	const auto get_network_list_result = WlanGetAvailableNetworkList(wlan_client, &wlan_interface_info->InterfaceGuid, 0, nullptr, &network_list);

	const auto isSuccess = WlanErrorWrapper::wrapGetNetworkListResult(get_network_list_result);

	if (!isSuccess)
		return nullptr;

	return network_list;
}

void WlanWrapper::connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry)
{
	const std::string authentication = "WPA2PSK";
	auto profile_xml = profile_helper->get_profile_xml(static_cast<std::string>(reinterpret_cast<char*>(entry.dot11Ssid.ucSSID)), authentication, "AES", "77777777");

	//TODO change this to automatically get wstring instead of string
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	std::wstring unicode_profile_xml = myconv.from_bytes(profile_xml);

	DWORD reasonCode;
	const auto set_profile_result = WlanSetProfile(wlan_client, &wlan_interface_info->InterfaceGuid, 0, reinterpret_cast<LPCWSTR>(unicode_profile_xml.c_str()), nullptr, true, nullptr, &reasonCode);
	WlanErrorWrapper::wrapSetProfileResult(set_profile_result, reasonCode);

	WLAN_CONNECTION_PARAMETERS params = {};
	params.dot11BssType = entry.dot11BssType;
	params.dwFlags = entry.dwFlags;

	params.pDesiredBssidList = 0;

	params.pDot11Ssid = &entry.dot11Ssid;
	params.strProfile = unicode_profile_xml.c_str();
	params.wlanConnectionMode = wlan_connection_mode_temporary_profile;

	auto connectResult = WlanConnect(wlan_client, &wlan_interface_info->InterfaceGuid, &params, nullptr);

	WlanErrorWrapper::wrapConnectResult(connectResult);
}

void WlanWrapper::connectToWifi()
{
	const auto network_list = getAvailableEntries();

	for (DWORD network_index = 0; network_index < network_list->dwNumberOfItems; network_index++)
	{
		WLAN_AVAILABLE_NETWORK network_entry = network_list->Network[network_index];
		
		if (strcmp(reinterpret_cast<char*>(network_entry.dot11Ssid.ucSSID), "Khomiak_na_XATI") == 0) {
			switch (network_entry.dot11DefaultAuthAlgorithm) {
			case DOT11_AUTH_ALGO_80211_OPEN:
				std::cout << "802.11 Open " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_80211_SHARED_KEY:
				std::cout << "802.11 Shared " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_WPA:
				std::cout << "WPA " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_WPA_PSK:
				std::cout << "WPA " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_RSNA_PSK:
				connect_to_rsnapsk(network_entry);
			default:
				break;
			}
		}
	}
}

WlanWrapper::WlanWrapper()
{
	profile_helper = new ProfileHelper();
	const auto init_result = initializeWlanClient();

	if (!init_result)
		exit(1);

	const auto wlan_info = initializeWlanInterfaceInfo();

	if (!wlan_info)
		exit(1);
}

WlanWrapper::~WlanWrapper()
{
	delete profile_helper;
}
