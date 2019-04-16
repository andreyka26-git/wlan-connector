#include "WlanApiWrapper.h"

void runTests() 
{
	WlanApiWrapper wlanWrapper;
	wlanWrapper.connect("77", "drabyqq007");
};

int main()
{
	runTests();
	std::system("pause");
}