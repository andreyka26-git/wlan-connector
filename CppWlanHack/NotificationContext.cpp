#include "NotificationContext.h"

void NotificationContext::set_ssid(std::string& ssid)
{
	this->ssid = ssid;
}

void NotificationContext::set_pass(std::string& pass)
{
	this->pass = pass;
}

std::string NotificationContext::get_ssid() const
{
	return ssid;
}

std::string NotificationContext::get_pass() const
{
	return pass;
}

NotificationContext::NotificationContext(const char* ssid, const char* pass)
{
	this->ssid = ssid;
	this->pass = pass;
}

NotificationContext::NotificationContext()
= default;

NotificationContext::~NotificationContext()
= default;
