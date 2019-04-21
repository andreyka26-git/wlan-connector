#include "WlanApiWrapper.h"
#include <wlanapi.h>
#include <iostream>
#include <wtypes.h>
#include <codecvt>
#include <algorithm>
#include <vector>

#include "WlanApiErrorWrapper.h"
#include "StringHelper.h"
#include "NotificationContext.h"

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

void WlanApiWrapper::wlan_notification_callback(PWLAN_NOTIFICATION_DATA notification_data, PVOID context)
{
	const auto authenticate_model = static_cast<NotificationContext*>(context);

	//WLAN_NOTIFICATION_SOURCE_MSM
	switch(notification_data->NotificationSource)
	{
	case WLAN_NOTIFICATION_SOURCE_MSM: std::cout << "WLAN_NOTIFICATION_SOURCE_MSM" << std::endl; break;
	default: std::cout << "Unknown notification source" << std::endl;
	}

	switch(notification_data->NotificationCode)
	{
	case wlan_notification_msm_connected:std::cout << authenticate_model->get_ssid() << " has pass: " << authenticate_model->get_pass() << std::endl; break;

	case wlan_notification_msm_associating:std::cout << "wlan_notification_msm_associating" << std::endl; break;

	case wlan_notification_msm_associated:std::cout << "wlan_notification_msm_associated" << std::endl; break;

	case wlan_notification_msm_authenticating:std::cout << "wlan_notification_msm_authenticating" << std::endl; break;

	case wlan_notification_msm_roaming_start:std::cout << "wlan_notification_msm_roaming_start" << std::endl; break;

	case wlan_notification_msm_radio_state_change:std::cout << "wlan_notification_msm_radio_state_change" << std::endl; break;

	case wlan_notification_msm_disassociating:std::cout << "wlan_notification_msm_disassociating" << std::endl; break;

	case wlan_notification_msm_disconnected:std::cout << "wlan_notification_msm_disconnected" << std::endl; break;

	case wlan_notification_msm_peer_join:std::cout << "wlan_notification_msm_peer_join" << std::endl; break;

	case wlan_notification_msm_peer_leave:std::cout << "wlan_notification_msm_peer_leave" << std::endl; break;

	case wlan_notification_msm_adapter_removal:std::cout << "wlan_notification_msm_adapter_removal" << std::endl; break;

	case wlan_notification_msm_adapter_operation_mode_change:std::cout << "wlan_notification_msm_adapter_operation_mode_change" << std::endl; break;

	default: std::cout << "unknown notification code"; break;
	}

	//delete authenticate_model;
}

bool WlanApiWrapper::connect(const char *ssid, const char * pass)
{
	ensure_entries();

	const auto network = std::find_if(std::begin(*entries), std::end(*entries), 
		[ssid](WLAN_AVAILABLE_NETWORK entry) 
	{
		return StringHelper::is_equal(entry.dot11Ssid.ucSSID, ssid);
	});

	if (network == std::end(*entries)) {
		std::cout << "Cannot find entry " << ssid << "break.";
		return false;
	}

	const auto connect_method = map_algorithm_to_method(*network);
	
	if(connect_method == nullptr)
	{
		std::cout << "cannot resolve connect method." << std::endl;
		return false;
	}

	(this->*connect_method)(*network, ssid, pass);

	//settings context
	//TODO make it smart pointer
	const auto notification_context  = new NotificationContext(ssid, pass);

	//registering callback on connecting
	const auto set_notification_result = WlanRegisterNotification(
		wlan_client,
		WLAN_NOTIFICATION_SOURCE_ALL,
		true,
		wlan_notification_callback,
		notification_context,
		nullptr,
		nullptr);

	return WlanApiErrorWrapper::wrap_set_connect_notification_result(set_notification_result);
}

WlanApiWrapper::connect_func WlanApiWrapper::map_algorithm_to_method(WLAN_AVAILABLE_NETWORK & network)
{
	switch (network.dot11DefaultAuthAlgorithm) {
	case DOT11_AUTH_ALGO_80211_OPEN:
		std::cout << "802.11 Open " << network.dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		std::cout << "802.11 Shared " << network.dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_WPA:
		std::cout << "WPA " << network.dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
		std::cout << "WPA " << network.dot11DefaultAuthAlgorithm << std::endl;
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
			return &WlanApiWrapper::try_connect_to_rsnapsk;
	default:
		break;
	}

	return nullptr;
}

bool WlanApiWrapper::try_connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry, const char *ssid, const char *pass)
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

	const auto params = build_wlan_parameters(entry, ssid, pass);
	const auto connect_result = WlanConnect(wlan_client, &wlan_interface_info->InterfaceGuid, params, nullptr);

	delete params;

	//check interface info isState property to connection mode. If connected, so write to file.
	if (!WlanApiErrorWrapper::wrap_connect_result(connect_result))
		return false;

	//TODO ensure delay

	return true;

	//update the interface in order to intelligently check 'isState'.
	/*if (!try_set_wlan_interface_info())
	{
		std::cout << "Cannot update wlan interface." << std::endl;
		return false;
	}

	if (wlan_interface_info->isState == wlan_interface_state_connected) {
		std::cout << ssid << " : " << pass << std::endl;
		return true;
	}

	return false;*/
}

PWLAN_CONNECTION_PARAMETERS WlanApiWrapper::build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, const char* ssid,
	const char* pass)
{
	const auto params = new WLAN_CONNECTION_PARAMETERS();

	params->wlanConnectionMode = wlan_connection_mode_profile;

	const std::string ssid_str(ssid);
	const auto w_ssid_str = StringHelper::convert_string_to_w_string(ssid_str);

	params->strProfile = static_cast<LPCWSTR>(w_ssid_str->c_str());
	params->pDot11Ssid = &entry.dot11Ssid;

	params->pDesiredBssidList = nullptr;
	params->dot11BssType = entry.dot11BssType;
	params->dwFlags = 0;

	return params;
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
