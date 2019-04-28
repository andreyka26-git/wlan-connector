#include "WlanApiWrapper.h"

int main(int argc, char** argv)
{
	WlanApiWrapper wlanWrapper;
	wlanWrapper.connect(argv[0], argv[1]);//"UKrtelecom_5E6B80", "UKR_5532"
	std::system("pause");
}