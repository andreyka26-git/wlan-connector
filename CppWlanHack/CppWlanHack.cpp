#include <iostream>
#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>
#include <fstream>
#include <string>
#include <codecvt>

#include "WlanWrapper.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

const std::string profile_file_name = "profile.xml";

void wrap_set_profile_result(int result_code, int reason_code);

void connect_to_wpapsk(WLAN_AVAILABLE_NETWORK &);
void connect_to_rsnapsk(HANDLE, WLAN_AVAILABLE_NETWORK &, PWLAN_INTERFACE_INFO);

void try_to_connect(const HANDLE &, PWLAN_INTERFACE_INFO);
std::string get_profile_xml(const std::string&, const std::string&, const std::string&, const std::string&);

PWLAN_INTERFACE_INFO getWlanInfo(HANDLE);
void show_available_entries(const HANDLE &, PWLAN_INTERFACE_INFO);

void free_wlan_resources(PWLAN_INTERFACE_INFO_LIST);
HANDLE initialize_wlan_client();

void replaceString(std::string& subject, const std::string& search, const std::string& replace);

int main()
{
	WlanWrapper wlanWrapper;
	wlanWrapper.connectToWifi();
	std::system("pause");
}

void try_to_connect(const HANDLE &wlan_client, PWLAN_INTERFACE_INFO wlan_interface)
{
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;
	WlanGetAvailableNetworkList(wlan_client, &wlan_interface->InterfaceGuid, 0, nullptr, &network_list);

	for (DWORD network_index = 0; network_index < network_list->dwNumberOfItems; network_index++)
	{
		WLAN_AVAILABLE_NETWORK network_entry = network_list->Network[network_index];

		if (strcmp(reinterpret_cast<char*>(network_entry.dot11Ssid.ucSSID), "UKrtelecom_5E6B80") == 0) {
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
					connect_to_wpapsk(network_entry);
					break;
				case DOT11_AUTH_ALGO_RSNA_PSK:
					connect_to_rsnapsk(wlan_client, network_entry, wlan_interface);
				default:
					break;
			}
		}
	}
}

void connect_to_rsnapsk(HANDLE wlanClient, WLAN_AVAILABLE_NETWORK & entry, PWLAN_INTERFACE_INFO wlan_interface) {
	const std::string authentication = "WPA2PSK";
	auto profile_xml = get_profile_xml(static_cast<std::string>(reinterpret_cast<char*>(entry.dot11Ssid.ucSSID)), authentication, "AES", "UKR_5532");

	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	std::wstring unicode_profile_xml = myconv.from_bytes(profile_xml);

	//TODO see profile template for appropriate type, reason code return "bad profile"
	DWORD reasonCode;
	const int set_profile_result = WlanSetProfile(wlanClient, &wlan_interface->InterfaceGuid, 0, reinterpret_cast<LPCWSTR>(unicode_profile_xml.c_str()), nullptr, true, nullptr, &reasonCode);
	wrap_set_profile_result(set_profile_result, reasonCode);

	WLAN_CONNECTION_PARAMETERS params = {};
	params.dot11BssType = entry.dot11BssType;
	params.dwFlags = entry.dwFlags;

	params.pDesiredBssidList = 0;

	params.pDot11Ssid = &entry.dot11Ssid;
	params.strProfile = unicode_profile_xml.c_str();
	params.wlanConnectionMode = wlan_connection_mode_temporary_profile;

	auto connectResult = WlanConnect(wlanClient, &wlan_interface->InterfaceGuid, &params, nullptr);

	if(connectResult != ERROR_SUCCESS)
		std::cout << "CANNOT CONNECT !\n" << std::endl;
	else
		std::cout << "CONNECT SUCCESS !\n" << std::endl;
}

void wrap_set_profile_result(const int result_code, const int reason_code)
{
	if (result_code != ERROR_SUCCESS) {
		std::cout << "some error is occured, error code: " << result_code << std::endl;

		switch (result_code) {
		case ERROR_ACCESS_DENIED: std::cout << "ACCESS DENIED"; break;
		case ERROR_ALREADY_EXISTS: std::cout << "ALREADY EXISTS"; break;
		case ERROR_BAD_PROFILE: std::cout << "BAD PROFILE"; break;
		case ERROR_INVALID_PARAMETER: std::cout << "INVALID PARAMETER"; break;
		case ERROR_NO_MATCH: std::cout << "NO MATCH"; break;
		default: std::cout << "other error"; break;
		}
		std::cout << std::endl;

		switch (reason_code)
		{
		case WLAN_REASON_CODE_SUCCESS: std::cout << "reason code: success"; break;
		case WLAN_REASON_CODE_UNKNOWN: std::cout << "reason code: unknown"; break;
			//configurationErrorCodes
		case WLAN_REASON_CODE_NETWORK_NOT_COMPATIBLE: std::cout << "reason code: network not compatible"; break;
		case WLAN_REASON_CODE_PROFILE_NOT_COMPATIBLE: std::cout << "reason code: profile is not compatible"; break;
			//connectionErrorCodes
		case WLAN_REASON_CODE_NO_AUTO_CONNECTION: std::cout << "reason code: no auto connection"; break;
		case WLAN_REASON_CODE_NOT_VISIBLE: std::cout << "reason code: network is not visible"; break;
		case WLAN_REASON_CODE_GP_DENIED: std::cout << "reason code: network is blocked by group policy"; break;
		case WLAN_REASON_CODE_USER_DENIED: std::cout << "reason code: network is blocked by the user"; break;
		case WLAN_REASON_CODE_BSS_TYPE_NOT_ALLOWED: std::cout << "reason code: basic service set (BSS) type is not allowed on this wireless adapter"; break;
		case WLAN_REASON_CODE_IN_FAILED_LIST: std::cout << "reason code: network is in the failed list"; break;
		case WLAN_REASON_CODE_IN_BLOCKED_LIST: std::cout << "reason code: network is in the blocked list"; break;
		case WLAN_REASON_CODE_SSID_LIST_TOO_LONG: std::cout << "reason code: size of the service set identifiers (SSID) list exceeds the maximum size supported by the adapter"; break;
		case WLAN_REASON_CODE_CONNECT_CALL_FAIL: std::cout << "reason code: media Specific Module (MSM) connect call fails"; break;
		case WLAN_REASON_CODE_SCAN_CALL_FAIL: std::cout << "reason code: MSM scan call fails"; break;
		case WLAN_REASON_CODE_NETWORK_NOT_AVAILABLE: std::cout << "reason code: network is not available. This reason code is also used when there is a mismatch between capabilities specified in an XML profile and interface and/or network capabilities"; break;
		case WLAN_REASON_CODE_PROFILE_CHANGED_OR_DELETED: std::cout << "reason code: profile was changed or deleted before the connection was established"; break;
		case WLAN_REASON_CODE_KEY_MISMATCH: std::cout << "reason code: profile key does not match the network key"; break;
		case WLAN_REASON_CODE_USER_NOT_RESPOND: std::cout << "reason code: user is not responding"; break;
		case WLAN_REASON_CODE_AP_PROFILE_NOT_ALLOWED_FOR_CLIENT: std::cout << "reason code: application tried to apply a wireless Hosted Network profile to a physical wireless network adapter using the WlanSetProfile function, rather than to a virtual device"; break;
		case WLAN_REASON_CODE_AP_PROFILE_NOT_ALLOWED: std::cout << "reason code: application tried to apply a wireless Hosted Network profile to a physical wireless network adapter using the WlanSetProfile function, rather than to a virtual device"; break;
			//profileValidationErrors
		case WLAN_REASON_CODE_INVALID_PROFILE_SCHEMA: std::cout << "reason code: profile invalid according to the schema"; break;
		case WLAN_REASON_CODE_PROFILE_MISSING: std::cout << "reason code: WLANProfile element is missing"; break;
		case WLAN_REASON_CODE_INVALID_PROFILE_NAME: std::cout << "reason code: name of the profile is invalid"; break;
		case WLAN_REASON_CODE_INVALID_PROFILE_TYPE: std::cout << "reason code: type of the profile is invalid"; break;
		case WLAN_REASON_CODE_INVALID_PHY_TYPE: std::cout << "reason code: PHY type is invalid"; break;
		case WLAN_REASON_CODE_MSM_SECURITY_MISSING: std::cout << "reason code: MSM security settings are missing"; break;
		case WLAN_REASON_CODE_IHV_SECURITY_NOT_SUPPORTED: std::cout << "reason code: independent hardware vendor (IHV) security settings are missing"; break;
		case WLAN_REASON_CODE_IHV_OUI_MISMATCH: std::cout << "reason code: IHV profile OUI did not match with the adapter OUI"; break;
		case WLAN_REASON_CODE_IHV_OUI_MISSING: std::cout << "reason code: IHV OUI settings are missing"; break;
		case WLAN_REASON_CODE_IHV_SETTINGS_MISSING: std::cout << "reason code: IHV security settings are missing"; break;
		case WLAN_REASON_CODE_IHV_CONNECTIVITY_NOT_SUPPORTED: std::cout << "reason code: application tried to apply an IHV profile on an adapter that does not support IHV connectivity settings"; break;
		case WLAN_REASON_CODE_CONFLICT_SECURITY: std::cout << "reason code: security settings conflict"; break;
		case WLAN_REASON_CODE_SECURITY_MISSING: std::cout << "reason code: security settings are missing"; break;
		case WLAN_REASON_CODE_INVALID_BSS_TYPE: std::cout << "reason code: BSS type is not valid"; break;
		case WLAN_REASON_CODE_INVALID_ADHOC_CONNECTION_MODE: std::cout << "reason code: automatic connection cannot be set for an ad hoc network"; break;
		case WLAN_REASON_CODE_NON_BROADCAST_SET_FOR_ADHOC: std::cout << "reason code: non-broadcast cannot be set for an ad hoc network"; break;
		case WLAN_REASON_CODE_AUTO_SWITCH_SET_FOR_ADHOC: std::cout << "reason code: auto-switch cannot be set for an ad hoc network"; break;
		case WLAN_REASON_CODE_AUTO_SWITCH_SET_FOR_MANUAL_CONNECTION: std::cout << "reason code: auto-switch cannot be set for a manual connection profile"; break;
		case WLAN_REASON_CODE_PROFILE_SSID_INVALID: std::cout << "reason code: the SSID in the profile is invalid or missing"; break;
		case WLAN_REASON_CODE_TOO_MANY_SSID: std::cout << "reason code: too many SSIDs were specified in the profile"; break;
		case WLAN_REASON_CODE_BAD_MAX_NUMBER_OF_CLIENTS_FOR_AP: std::cout << "reason code: application tried to apply a wireless Hosted Network profile to a physical network adapter NIC using the WlanSetProfile function, and specified an unacceptable value for the maximum number of clients allowed"; break;
		case WLAN_REASON_CODE_INVALID_CHANNEL: std::cout << "reason code: channel specified is invalid"; break;
		case WLAN_REASON_CODE_AUTO_AP_PROFILE_NOT_ALLOWED: std::cout << "reason code: internal operating system error occurred with the wireless Hosted Network"; break;
		default: std::cout << "unknown error."; break;
			//END PROFILE ERRORS
		}
	}
}

void connect_to_wpapsk(WLAN_AVAILABLE_NETWORK & entry) {
	const std::string authentication = "WPAPSK";
	auto profile_xml = get_profile_xml(static_cast<std::string>(reinterpret_cast<char*>(entry.dot11Ssid.ucSSID)), authentication, "AES", "UKR_5532");
}

std::string get_profile_xml(const std::string &profile_name, const std::string &authentication, const std::string &encryption, const std::string &key) {
	std::ifstream xml(profile_file_name);
	std::string xmlContent;

	if (xml) {
		xmlContent = std::string((std::istreambuf_iterator<char>(xml)),
			std::istreambuf_iterator<char>());

		replaceString(xmlContent, "<name>{0}</name>", "<name>" + profile_name + "</name>");
		replaceString(xmlContent, "<authentication>{1}</authentication>", "<authentication>" + authentication + "</authentication>");
		replaceString(xmlContent, "<encryption>{2}</encryption>", "<encryption>" + encryption + "</encryption>");
		replaceString(xmlContent, "<keyMaterial>{3}</keyMaterial>", "<keyMaterial>" + key + "</keyMaterial>");
	}

	xml.close();
	return xmlContent;
}

void show_available_entries(const HANDLE &wlan_client, PWLAN_INTERFACE_INFO wlan_interface)
{
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;
	WlanGetAvailableNetworkList(wlan_client, &wlan_interface->InterfaceGuid, 0, nullptr, &network_list);

	for (DWORD networkIndex = 0; networkIndex < network_list->dwNumberOfItems; networkIndex++) {
		auto network_entry = network_list->Network[networkIndex];

		std::cout << "Profile name: " << network_entry.strProfileName << std::endl;

		std::cout << "SSID: ";
		if (network_entry.dot11Ssid.uSSIDLength != 0)
			std::cout << network_entry.dot11Ssid.ucSSID << std::endl;

		std::cout << "BSS Network type: " << network_entry.dot11BssType << std::endl;
		std::cout << "Number of BSSIDs: " << network_entry.uNumberOfBssids << std::endl;

		if (network_entry.bNetworkConnectable)
			std::cout << "It's connectable." << std::endl;

		std::cout << "  Default AuthAlgorithm: ";
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
				std::cout << "WPA-PSK " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_WPA_NONE:
				std::cout << "WPA-None " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_RSNA_PSK:
				std::cout << "RSNA with PSK" << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			case DOT11_AUTH_ALGO_RSNA:
				std::cout << "RSNA " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
			default:
				std::cout << "Other " << network_entry.dot11DefaultAuthAlgorithm << std::endl;
				break;
		}
		std::cout << std::endl;
	}
}

PWLAN_INTERFACE_INFO getWlanInfo(HANDLE wlanClient) {
	PWLAN_INTERFACE_INFO_LIST interfacesList = nullptr;
	const auto enum_interfaces_result = WlanEnumInterfaces(wlanClient, nullptr, &interfacesList);

	if (enum_interfaces_result != ERROR_SUCCESS) {
		std::cout << "WlanEnumInterfacesfailed with errors: " << enum_interfaces_result << std::endl;
		return nullptr;
	}

	std::cout << std::endl << "Num of entries: " << interfacesList->dwNumberOfItems << std::endl;
	std::cout << std::endl << "Current index: " << interfacesList->dwIndex << std::endl;

	return interfacesList->InterfaceInfo;
}

HANDLE initialize_wlan_client() {
	HANDLE h_client = nullptr;
	const DWORD dwMaxClient = 2;
	DWORD dw_cur_version = 0;

	const auto openHandleResult = WlanOpenHandle(dwMaxClient, nullptr, &dw_cur_version, &h_client);

	if (openHandleResult != ERROR_SUCCESS) {
		std::cout << "WlanOpenHandle failed with error: " << openHandleResult << std::endl;
		return nullptr;
	}
	return h_client;
}

void replaceString(std::string & subject, const std::string & search, const std::string & replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

void free_wlan_resources(PWLAN_INTERFACE_INFO_LIST wlanInterfaces) {
	WlanFreeMemory(wlanInterfaces);
	wlanInterfaces = nullptr;
}