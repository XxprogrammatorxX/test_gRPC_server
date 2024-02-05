#pragma once

#include <string>

struct DeviceSpec
{
	std::string name;
	std::string OS_version;
	std::string serial_number;
};

DeviceSpec get_device_spec();
