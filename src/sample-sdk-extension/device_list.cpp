#include "device_list.h"

#include <stdio.h>
#include <cstdio>


static bool write_device_set_to_file(const std::set<std::string> & devices,
	const std::string & path, std::string * last_error_ptr)
{
	std::FILE * file = std::fopen(path.c_str(), "w");
	if (file == nullptr)
	{
		*last_error_ptr = "ERROR opening " + std::string(path) + " file.";
		return false;
	}
	for (const std::string & device : devices)
	{
		if (std::fputs(device.c_str(), file) == EOF)
		{
			*last_error_ptr = "ERROR writing to " + std::string(path)
				+ " file.";
			return false;
		}
		if (std::fputs("\n", file) == EOF)
		{
			*last_error_ptr = "ERROR writing to " + std::string(path)
				+ " file.";
			return false;
		}
	}
	if (std::fclose(file) != 0)
	{
		*last_error_ptr = "ERROR closing the file handle";
		return false;
	}
	return true;
}

static bool append_device_to_file(const std::string & device,
	const std::string & path, std::string * last_error_ptr)
{
	std::FILE * file = std::fopen(path.c_str(), "a");
	if (file == nullptr)
	{
		*last_error_ptr =
			"ERROR opening " + std::string(path) + " file.";
		return false;
	}
	int r = std::fputs(device.c_str(), file);
	if (r == EOF)
	{
		*last_error_ptr =
			"ERROR writing to " + std::string(path) + " file.";
		return false;
	}
	if (std::fclose(file) != 0)
	{
		*last_error_ptr = "ERROR closing the file handle";
		return false;
	}
	return true;
}

std::string DeviceList::pull_last_error()
{
	return std::move(m_last_error);
}

std::string DeviceList::get_last_error() const
{
	return m_last_error;
}

bool DeviceList::has_error() const {
	if (m_last_error.empty()) {
		return false;
	}
	return true;
}

bool DeviceList::load_from_file(const std::string & path)
{
	std::FILE * file = std::fopen(path.c_str(), "r");
	if (file == NULL)
	{
		m_last_error = "ERROR opening " + std::string(path) + " file.";
		return false;
	}
	char * line = NULL;
	size_t len = 0;
	while (::getline(&line, &len, file) != 1)
	{
		m_devices.insert(std::string(line));
	}
	::free(line);
	if (::feof(file) == 0)
	{
		m_last_error = "ERROR reading " + std::string(path) + " file.";
		return false;
	}
	if (std::fclose(file) != 0)
	{
		m_last_error = "ERROR closing the file handle";
		return false;
	}
	return true;
}

bool DeviceList::add(const std::string & device)
{
	auto result = m_devices.insert(device);
	if (result.second)
	{
		return append_device_to_file(device, m_file_path, &m_last_error);
	}
	return false;
}

bool DeviceList::remove(const std::string & device)
{
	if (m_devices.erase(device) > 0)
	{
		return write_device_set_to_file(m_devices, m_file_path, &m_last_error);
	}
	return true;
}

bool DeviceList::is_in_the_list(const std::string & device) const
{
	if (m_devices.find(device) == m_devices.end())
	{
		return false;
	}
	return true;
}
