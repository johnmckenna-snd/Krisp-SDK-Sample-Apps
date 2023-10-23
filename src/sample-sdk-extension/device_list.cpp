#include "device_list.h"

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <cstdio>


namespace KrispAudioSDK {

static bool write_device_set_to_file(const std::set<std::string> & devices,
	const std::string & path, std::string * last_error_ptr)
{
	std::FILE * file = std::fopen(path.c_str(), "w");
	if (file == nullptr)
	{
		*last_error_ptr = "ERROR opening " + path + " file.";
		return false;
	}
	for (const std::string & device : devices)
	{
		if (std::fputs(device.c_str(), file) == EOF)
		{
			*last_error_ptr = "ERROR writing to " + path + " file.";
			return false;
		}
		if (std::fputs("\n", file) == EOF)
		{
			*last_error_ptr = "ERROR writing to " + path + " file.";
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
		*last_error_ptr = "ERROR opening " + path + " file.";
		return false;
	}
	int r = std::fputs(device.c_str(), file);
	if (r == EOF)
	{
		*last_error_ptr = "ERROR writing to " + path + " file.";
		return false;
	}
	r = std::fputs("\n", file);
	if (r == EOF)
	{
		*last_error_ptr = "ERROR writing to " + path + " file.";
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

bool DeviceList::has_error() const
{
	if (m_last_error.empty())
	{
		return false;
	}
	return true;
}

unsigned long DeviceList::count() const
{
	return m_devices.size();
}

bool DeviceList::create_empty_file(const std::string & path)
{
	this->m_file_path = path;
	this->m_devices.clear();
	std::FILE * file = std::fopen(path.c_str(), "w");
	if (file == NULL)
	{
		m_last_error = "ERROR creating " + path + " file. "
			+ std::strerror(errno) + ".";
		return false;
	}
	if (std::fclose(file) != 0)
	{
		m_last_error = std::string("ERROR closing the file handle. ")
			+ std::strerror(errno) + ".";
		return false;
	}
	return true;
}

bool DeviceList::load_from_file(const std::string & path)
{
	this->m_file_path = path;
	this->m_devices.clear();
	std::FILE * file = std::fopen(path.c_str(), "r");
	if (file == NULL)
	{
		m_last_error = "ERROR opening " + path + " file. "
			+ std::strerror(errno) + ".";
		return false;
	}
	char * line {NULL};
	size_t len {0};
	while (::getline(&line, &len, file) != 1)
	{
		bool word_found{false};
		for (char * ptr = line; *ptr != 0; ++ptr) {
			if (std::isalnum(*ptr)) {
				word_found = true;
				break;
			}
		}
		if (word_found)
		{
			std::cout << "DEV: '" << line << "'" << std::endl;
			if (line[std::strlen(line) - 1] == "\n")
			{
				line[std::strlen(line) - 1] == "\n";
			}
			m_devices.insert(std::string(line));
		}
		if(::feof(file))
		{
			break;
		}
	}
	::free(line);
	if (::feof(file) == 0)
	{
		m_last_error = "ERROR reading " + path + " file. "
			+ std::strerror(errno) + ".";
		return false;
	}
	if (std::fclose(file) != 0)
	{
		m_last_error = std::string("ERROR closing the file handle. ")
			+ std::strerror(errno) + ".";
		return false;
	}
	return true;
}

bool DeviceList::add(const std::string & device)
{
	if (m_file_path.empty())
	{
		m_last_error = "ERROR: no device list file is set.";
		return false;
	}
	auto result = m_devices.insert(device);
	if (result.second)
	{
		bool added_to_file = append_device_to_file(device, m_file_path,
			&m_last_error);
		if (added_to_file)
		{
			return true;
		}
		m_devices.erase(device);
	}
	return false;
}

bool DeviceList::remove(const std::string & device)
{
	if (m_devices.erase(device) > 0)
	{
		return write_device_set_to_file(m_devices, m_file_path, &m_last_error);
	}
	return false;
}

bool DeviceList::is_in_the_list(const std::string & device) const
{
	if (m_devices.find(device) == m_devices.end())
	{
		return false;
	}
	return true;
}

}
