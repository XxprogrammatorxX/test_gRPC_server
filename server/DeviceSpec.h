#pragma once

#include <string>
#include <fstream>
#include <regex>

struct DeviceSpec
{
	std::string name;			// cat /etc/hostname 
	std::string OS_version;		// cat /etc/os-release
	std::string serial_number;	// sudo cat /sys/devices/virtual/dmi/id/chassis_serial
};

std::string get_device_name()
{
	std::string path = "/etc/hostname";
	std::ifstream input(path, std::ios::in);
	if (!input.good())
		return "Unable to get";
	std::string name;
	std::getline (input, name);
	return name.empty() ? "Unable to get" : name;
}

std::string get_device_OS_version()
{
	std::string path = "/etc/os-release";
	std::ifstream input(path, std::ios::in);
	if (!input.good())
		return "Unable to get";

	const std::regex pieces_regex(R"(PRETTY_NAME="*")");
	std::smatch pieces_match;

	std::string line;
	std::string version;
	while (std::getline (input, line))
		if (std::regex_match(line, pieces_match, pieces_regex))
			version = pieces_match[1];

	return version.empty() ? "Unable to get" : version;
}

std::string get_device_serial_number()
{
	std::string path = "/sys/devices/virtual/dmi/id/chassis_serial";
	std::ifstream input(path, std::ios::in);
	if (!input.good())
		return "Unable to get";

	std::string serial;
	std::getline (input, serial);
	return serial.empty() ? "Unable to get" : serial;
}

DeviceSpec get_device_spec()
{
	DeviceSpec spec;
	spec.name = get_device_name();
	spec.OS_version = get_device_OS_version();
	spec.serial_number = get_device_serial_number();
	return spec;
}
