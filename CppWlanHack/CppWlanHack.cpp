// WlanHack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <fstream>
#include <string>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

const std::string profileFileName = "profile.xml";

void connectToWPAPSK(WLAN_AVAILABLE_NETWORK &);
void connectToRSNAPSK(WLAN_AVAILABLE_NETWORK &);

void tryToConnect(HANDLE, PWLAN_INTERFACE_INFO);
std::string getProfileXml(std::string, std::string, std::string, std::string);

PWLAN_INTERFACE_INFO getWlanInfo(HANDLE);
void showAvailableEntries(HANDLE, PWLAN_INTERFACE_INFO);

void freeWlanResources(PWLAN_INTERFACE_INFO_LIST);
HANDLE initializeWlanClient();

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

	for (int networkIndex = 0; networkIndex < networkList->dwNumberOfItems; networkIndex++)
	{
		WLAN_AVAILABLE_NETWORK networkEntry = networkList->Network[networkIndex];

		if (strcmp((char*)networkEntry.dot11Ssid.ucSSID, "UKrtelecom_5E6B80") == 0) {
			switch (networkEntry.dot11DefaultAuthAlgorithm) {
				case DOT11_AUTH_ALGO_80211_OPEN:
					std::cout << "802.11 Open " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
					break;
				case DOT11_AUTH_ALGO_80211_SHARED_KEY:
					std::cout << "802.11 Shared " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
					break;
				case DOT11_AUTH_ALGO_WPA:
					std::cout << "WPA " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
					break;
				case DOT11_AUTH_ALGO_WPA_PSK:
					connectToWPAPSK(networkEntry);
					break;
				case DOT11_AUTH_ALGO_RSNA_PSK:
					connectToRSNAPSK(networkEntry);
				default:
					break;
			}
		}
	}
}

void connectToRSNAPSK(WLAN_AVAILABLE_NETWORK & entry) {
	std::string authentication = "RSNAPSK";
	std::string profileXml = getProfileXml((std::string)(char*)entry.dot11Ssid.ucSSID, authentication, std::to_string(entry.dot11DefaultCipherAlgorithm), "UKR_5532");

}

void connectToWPAPSK(WLAN_AVAILABLE_NETWORK &entry) {
	std::string authentication = "WPAPSK";
	std::string profileXml = getProfileXml((std::string)(char*)entry.dot11Ssid.ucSSID, authentication, std::to_string(entry.dot11DefaultCipherAlgorithm), "UKR_5532");
	
}

std::string getProfileXml(std::string profileName, std::string authentication, std::string encryption, std::string key) {
	std::ifstream xml(profileFileName);
	std::string xmlContent;

	if (xml) {
		xmlContent = std::string((std::istreambuf_iterator<char>(xml)),
			std::istreambuf_iterator<char>());
	}
	else
		return std::string();

	return xmlContent;
}

void showAvailableEntries(HANDLE wlanClient, PWLAN_INTERFACE_INFO wlanInterface)
{
	PWLAN_AVAILABLE_NETWORK_LIST networkList = nullptr;
	int networkListResult = WlanGetAvailableNetworkList(wlanClient, &wlanInterface->InterfaceGuid, 0, nullptr, &networkList);

	for (int networkIndex = 0; networkIndex < networkList->dwNumberOfItems; networkIndex++) {
		WLAN_AVAILABLE_NETWORK networkEntry = networkList->Network[networkIndex];

		std::cout << "Profile name: " << networkEntry.strProfileName << std::endl;

		std::cout << "SSID: ";
		if (networkEntry.dot11Ssid.uSSIDLength != 0)
			std::cout << networkEntry.dot11Ssid.ucSSID << std::endl;

		std::cout << "BSS Network type: " << networkEntry.dot11BssType << std::endl;
		std::cout << "Number of BSSIDs: " << networkEntry.uNumberOfBssids << std::endl;

		if (networkEntry.bNetworkConnectable)
			std::cout << "It's connectable." << std::endl;

		std::cout << "  Default AuthAlgorithm: ";
		switch (networkEntry.dot11DefaultAuthAlgorithm) {
			case DOT11_AUTH_ALGO_80211_OPEN:
				std::cout << "802.11 Open " << networkEntry.dot11DefaultAuthAlgorithm << std::endl;
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

void freeWlanResources(PWLAN_INTERFACE_INFO_LIST wlanInterfaces) {
	WlanFreeMemory(wlanInterfaces);
	wlanInterfaces = nullptr;
}