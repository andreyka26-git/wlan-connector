#include "WlanErrorWrapper.h"
#include <iostream>
#include <windows.h>

bool WlanErrorWrapper::wrapOpenHandleResult(const DWORD open_handle_result)
{
	if (open_handle_result != ERROR_SUCCESS) {
		std::cout << "WlanOpenHandle failed with error: " << open_handle_result << std::endl;
		return false;
	}

	return true;
}

bool WlanErrorWrapper::wrapEnumInterfacesResult(const DWORD enum_interfaces_result)
{
	if (enum_interfaces_result != ERROR_SUCCESS) {
		std::cout << "WlanEnumInterfacesfailed with errors: " << enum_interfaces_result << std::endl;
		return false;
	}

	return true;
}

bool WlanErrorWrapper::wrapGetNetworkListResult(const DWORD get_network_list_result)
{
	if (get_network_list_result != ERROR_SUCCESS) {
		std::cout << "WlanGetAvailableNetworkList with errors: " << get_network_list_result << std::endl;
		return false;
	}

	return true;
}

bool WlanErrorWrapper::wrapSetProfileResult(const DWORD result_code, const DWORD reason_code)
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
		return false;
	}
	return true;
}

bool WlanErrorWrapper::wrapConnectResult(DWORD connectResult)
{
	if (connectResult != ERROR_SUCCESS) {

		std::cout << "CANNOT CONNECT !\n" << std::endl;

		switch (connectResult) {
		case ERROR_INVALID_PARAMETER: std::cout << "Some parameters is invalid." << std::endl; break;
		case ERROR_INVALID_HANDLE: std::cout << "Invalid handle." << std::endl; break;
		case ERROR_ACCESS_DENIED: std::cout << "The caller does not have sufficient permissions." << std::endl; break;
		default: std::cout << "Some other kind of error." << std::endl; break;
		}
		
		return false;
	}
	return true;
}

WlanErrorWrapper::WlanErrorWrapper()
{
}

WlanErrorWrapper::~WlanErrorWrapper()
{
}
