#pragma once
#include <iostream>

class NotificationContext
{
	std::string ssid;
	std::string pass;
public:
	void set_ssid(std::string& ssid);
	void set_pass(std::string& pass);

	std::string get_ssid() const;
	std::string get_pass() const;

	NotificationContext(const char * ssid, const char * pass);
	NotificationContext();
	~NotificationContext();
};

