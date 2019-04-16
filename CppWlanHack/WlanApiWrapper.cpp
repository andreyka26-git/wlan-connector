#include "WlanApiWrapper.h"
#include <wlanapi.h>
#include <iostream>
#include <wtypes.h>
#include <codecvt>
#include <algorithm>
#include <vector>

#include "WlanApiErrorWrapper.h"
#include "StringHelper.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

void WlanApiWrapper::scan_entries() const
{
	const auto network_list = getAvailableEntries();

	for (DWORD network_index = 0; network_index < network_list->dwNumberOfItems; network_index++) {
		entries->push_back(network_list->Network[network_index]);
	}

	WlanFreeMemory(network_list);
}

bool WlanApiWrapper::connect(const char *ssid, const char * pass)
{
	ensure_entries();

	//TODO suppress line
	const auto network = std::find_if(std::begin(*entries), std::end(*entries), [ssid](WLAN_AVAILABLE_NETWORK entry) 
	{
		return StringHelper::is_equal(entry.dot11Ssid.ucSSID, ssid);
	});

	if (network == std::end(*entries)) {
		std::cout << "Cannot find entry " << ssid << "break.";
		return false;
	}

	switch ((*network).dot11DefaultAuthAlgorithm) {
	case DOT11_AUTH_ALGO_80211_OPEN:
		std::cout << "802.11 Open " << (*network).dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		std::cout << "802.11 Shared " << (*network).dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_WPA:
		std::cout << "WPA " << (*network).dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
		std::cout << "WPA " << (*network).dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
		return connect_to_rsnapsk(*network, ssid, pass);
	default:
		break;
	}
	return false;
}

bool WlanApiWrapper::connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry, const char *ssid, const char *pass)
{
	const auto profile_xml =
		profile_helper->get_profile_xml(ssid, DefaultAuthenticationProtocol, DefaultEncryption, pass);

	//need because setProfile uses prfile_xml in unicode.
	const auto wprofile_xml = StringHelper::convert_string_to_w_string(*profile_xml);

	DWORD reason_code;
	const auto set_profile_result = WlanSetProfile(wlan_client, &wlan_interface_info->InterfaceGuid, 0, (*wprofile_xml).c_str(), nullptr, true, nullptr, &reason_code);

	if (!WlanApiErrorWrapper::wrap_set_profile_result(set_profile_result, reason_code)) {
		std::cout << "Cannot set profile. Continue trying...";
		return false;
	}

	//TODO replace to single method
	WLAN_CONNECTION_PARAMETERS params;

	params.wlanConnectionMode = wlan_connection_mode_profile;

	const std::string ssid_str(ssid);
	const auto w_ssid_str = StringHelper::convert_string_to_w_string(ssid_str);

	params.strProfile = static_cast<LPCWSTR>(w_ssid_str->c_str());
	params.pDot11Ssid = &entry.dot11Ssid;

	params.pDesiredBssidList = nullptr;
	params.dot11BssType = entry.dot11BssType;
	params.dwFlags = 0;

	const auto connect_result = WlanConnect(wlan_client, &wlan_interface_info->InterfaceGuid, &params, nullptr);

	//check interface info isState property to connection mode. If connected, so write to file.
	if (!WlanApiErrorWrapper::wrap_connect_result(connect_result))
		return false;

	//TODO ensure delay

	//update the interface in order to intelligently check 'isState'.
	if (!try_set_wlan_interface_info())
	{
		std::cout << "Cannot update wlan interface." << std::endl;
		return false;
	}

	if (wlan_interface_info->isState == wlan_interface_state_connected) {
		std::cout << ssid << " : " << pass << std::endl;
		return true;
	}

	return false;
}

bool WlanApiWrapper::try_set_wlan_client()
{
	const DWORD dw_max_client = 2;
	DWORD dw_cur_version = 0;

	const auto open_handle_result = WlanOpenHandle(dw_max_client, nullptr, &dw_cur_version, &wlan_client);
	const auto is_success = WlanApiErrorWrapper::wrap_open_handle_result(open_handle_result);

	return is_success;
}

bool WlanApiWrapper::try_set_wlan_interface_info()
{
	PWLAN_INTERFACE_INFO_LIST interfaces_list = nullptr;
	const auto enum_interfaces_result = WlanEnumInterfaces(wlan_client, nullptr, &interfaces_list);

	const auto is_success = WlanApiErrorWrapper::wrap_enum_interfaces_result(enum_interfaces_result);

	wlan_interface_info = interfaces_list->InterfaceInfo;
	return is_success;
}

PWLAN_AVAILABLE_NETWORK_LIST WlanApiWrapper::getAvailableEntries() const
{
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;

	const auto get_network_list_result =
		WlanGetAvailableNetworkList(wlan_client, &wlan_interface_info->InterfaceGuid, 0, nullptr, &network_list);

	const auto is_success = WlanApiErrorWrapper::wrap_get_network_list_result(get_network_list_result);

	if (!is_success)
		return nullptr;

	return network_list;
}

void WlanApiWrapper::ensure_entries() const
{
	if (entries->empty())
		scan_entries();

	//cannot find any wifi entry; exit;
	if (entries->empty())
	{
		std::cout << "There are none of wifi entries. Break.";
		exit(0);
	}
}

WlanApiWrapper::WlanApiWrapper()
{
	entries = new std::vector<WLAN_AVAILABLE_NETWORK>();

	profile_helper = new ProfileHelper();
	const auto init_result = try_set_wlan_client();

	if (!init_result)
		exit(1);

	const auto wlan_info = try_set_wlan_interface_info();

	if (!wlan_info)
		exit(1);
}

WlanApiWrapper::~WlanApiWrapper()
{
	delete entries;

	delete profile_helper;
}
