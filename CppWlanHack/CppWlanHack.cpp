#include "WlanApiWrapper.h"

void runTests() 
{
	WlanApiWrapper wlanWrapper; 
	wlanWrapper.connect("UKrtelecom_5E6B80", "UKR_5532");
};

int main()
{
	runTests();
	std::system("pause");
}