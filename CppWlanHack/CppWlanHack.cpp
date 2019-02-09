// WlanHack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

HANDLE initializeWlanClient();

PWLAN_INTERFACE_INFO getWlanInfo(HANDLE);

void showInterfacesInfo(PWLAN_INTERFACE_INFO_LIST);

void tryToConnect(HANDLE, PWLAN_INTERFACE_INFO);

void freeWlanResources(PWLAN_INTERFACE_INFO_LIST);

int main()
{
	HANDLE wlanClient = initializeWlanClient();
	PWLAN_INTERFACE_INFO wlanInfo = getWlanInfo(wlanClient);

	tryToConnect(wlanClient, wlanInfo);
	
	std::system("pause");
}

void tryToConnect(HANDLE wlanClient, PWLAN_INTERFACE_INFO wlanInterface)
{
	PWLAN_AVAILABLE_NETWORK_LIST networkList = nullptr;

	int networkListResult = WlanGetAvailableNetworkList(wlanClient, &wlanInterface->InterfaceGuid, 0, nullptr, &networkList);

	for (int networkIndex = 0; networkIndex < networkList->dwNumberOfItems; networkIndex++) {
		WLAN_AVAILABLE_NETWORK networkEntry = networkList->Network[networkIndex];

		if (strcmp((char *)networkEntry.dot11Ssid.ucSSID, "UKrtelecom_5E6B80") == 0) {
			//TODO 
			// As I researched, I need to set profile with appropriate xml with appropriate key(password)
			// and then just try to connect with this profile....
			PWLAN_CONNECTION_PARAMETERS parameters = new WLAN_CONNECTION_PARAMETERS();

			parameters->dot11BssType = networkEntry.dot11BssType;
			parameters->pDot11Ssid = &networkEntry.dot11Ssid;
			parameters->strProfile = networkEntry.strProfileName;
			parameters->wlanConnectionMode = WLAN_CONNECTION_MODE::wlan_connection_mode_temporary_profile;
			parameters->dwFlags = 0;

			int connectResult = WlanConnect(wlanClient, &wlanInterface->InterfaceGuid, parameters, nullptr);
			if (connectResult != ERROR_SUCCESS) {
				std::cout << "Connecting error " << connectResult << std::endl;
			}
		}

		std::cout << "Profile name: " << networkEntry.strProfileName << std::endl;

		std::cout << "SSID: ";
		if (networkEntry.dot11Ssid.uSSIDLength != 0)
			std::cout << networkEntry.dot11Ssid.ucSSID << std::endl;

		std::cout << "BSS Network type: " << networkEntry.dot11BssType << std::endl;
		std::cout << "Number of BSSIDs: " << networkEntry.uNumberOfBssids << std::endl;

		if (networkEntry.bNetworkConnectable)
			std::cout << "It's connectable." << std::endl;

		//authentication
		std::cout <<"  Default AuthAlgorithm: ";
		switch (networkEntry.dot11DefaultAuthAlgorithm) {
		case DOT11_AUTH_ALGO_80211_OPEN:
			std::cout << "802.11 Open "<< networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_80211_SHARED_KEY:
			std::cout << "802.11 Shared " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_WPA:
			std::cout << "WPA " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_WPA_PSK:
			std::cout << "WPA-PSK " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_WPA_NONE:
			std::cout << "WPA-None " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_RSNA:
			std::cout << "RSNA " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		case DOT11_AUTH_ALGO_RSNA_PSK:
			std::cout << "RSNA with PSK" << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		default:
			std::cout << "Other " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
			break;
		}
		std::cout << std::endl;
	}
}

PWLAN_INTERFACE_INFO getWlanInfo(HANDLE wlanClient) {
	PWLAN_INTERFACE_INFO_LIST interfacesList = nullptr;
	DWORD enumInterfacesResult = WlanEnumInterfaces(wlanClient, nullptr, &interfacesList);

	if (enumInterfacesResult != ERROR_SUCCESS) {
		std::cout << "WlanEnumInterfacesfailed with errors: " << enumInterfacesResult << std::endl;
		return nullptr;
	}

	std::cout << std::endl << "Num of entries: " << interfacesList->dwNumberOfItems << std::endl;
	std::cout << std::endl << "Current index: " << interfacesList->dwIndex << std::endl;

	return interfacesList->InterfaceInfo;
}

HANDLE initializeWlanClient() {
	HANDLE hClient = nullptr;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;

	DWORD openHandleResult = WlanOpenHandle(dwMaxClient, nullptr, &dwCurVersion, &hClient);

	if (openHandleResult != ERROR_SUCCESS) {
		std::cout << "WlanOpenHandle failed with error: " << openHandleResult << std::endl;
		return nullptr;
	}
	return hClient;
}

void showInterfacesInfo(PWLAN_INTERFACE_INFO_LIST wlanInterfaces) {
	for (int interfaceIndex = 0; interfaceIndex < wlanInterfaces->dwNumberOfItems; interfaceIndex++) {
		WLAN_INTERFACE_INFO currentInfo = wlanInterfaces->InterfaceInfo[interfaceIndex];

		WCHAR guidString[40] = { 0 };

		int getGuidResult = StringFromGUID2(currentInfo.InterfaceGuid, (LPOLESTR)guidString, 39);

		if (getGuidResult == 0)
			std::cout << "Getting guid failed." << std::endl;

		std::cout << "Interface guid: " << guidString << std::endl;
		std::cout << "Interface description: " << currentInfo.strInterfaceDescription << std::endl;

		std::cout << "State: " << std::endl;

		switch (currentInfo.isState)
		{
		case wlan_interface_state_not_ready: std::cout << "NOT READY";
			break;
		case wlan_interface_state_connected: std::cout << "CONNECTED";
			break;
		case wlan_interface_state_disconnected: std::cout << "DISCONNECTED";
			break;
		case wlan_interface_state_authenticating: std::cout << "DISCONNECTED";
			break;
		default: std::cout << "Cannot determine.";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void freeWlanResources(PWLAN_INTERFACE_INFO_LIST wlanInterfaces) {
	WlanFreeMemory(wlanInterfaces);
	wlanInterfaces = nullptr;
}