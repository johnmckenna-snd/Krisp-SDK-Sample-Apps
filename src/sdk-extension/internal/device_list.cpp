#include "device_list.h"

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <cstdio>

#include "krisp-exception.h"


namespace KrispVoiceSDK {


struct FileOpener final
{
	~FileOpener()
	{
		if (m_file)
		{
			std::fclose(m_file);
		}
	}

	void open (const std::string & path, const char * mode)
	{
		if (m_file)
		{
			close();
		}
		m_file = std::fopen(path.c_str(), mode);
		if (m_file == nullptr)
		{
			throw KrispDeviceListError("ERROR accessing " + path + " file in " + mode + " mode. " +
				std::strerror(errno) + ".");
		}
	}

	void close()
	{
		if (m_file)
		{
			if (std::fclose(m_file) != 0)
			{
				throw KrispDeviceListError(std::string("ERROR closing the file handle. ") +
					std::strerror(errno) + ".");
			}
			m_file = nullptr;
		}
	}

	std::FILE * m_file = nullptr;
};


static void write_device_set_to_file(const std::set<std::string> & devices, const std::string & path)
{
	FileOpener fo;
	fo.open(path, "w");
	for (const std::string & device : devices)
	{
		if (std::fputs(device.c_str(), fo.m_file) == EOF)
		{
			throw KrispDeviceListError("ERROR writing to " + path + " file.");
		}
		if (std::fputs("\n", fo.m_file) == EOF)
		{
			throw KrispDeviceListError("ERROR writing to " + path + " file.");
		}
	}
	fo.close();
}

static void append_device_to_file(const std::string & device, const std::string & path)
{
	FileOpener fo;
	fo.open(path, "a");
	int r = std::fputs(device.c_str(), fo.m_file);
	if (r == EOF)
	{
		throw KrispDeviceListError("ERROR writing to " + path + " file.");
	}
	r = std::fputs("\n", fo.m_file);
	if (r == EOF)
	{
		throw KrispDeviceListError("ERROR writing to " + path + " file.");
	}
	fo.close();
}

unsigned long DeviceList::count() const
{
	return m_devices.size();
}

void DeviceList::create_empty_file(const std::string & path)
{
	this->m_file_path = path;
	this->m_devices.clear();
	FileOpener fo;
	fo.open(path, "w");
	fo.close();
}

void DeviceList::load_from_file(const std::string & path)
{
	this->m_file_path = path;
	this->m_devices.clear();
	FileOpener fo;
	fo.open(path, "r");
	char * line = nullptr;
	size_t len = 0;
	while (::getline(&line, &len, fo.m_file) != -1)
	{
		bool word_found = false;
		for (char * ptr = line; *ptr != 0; ++ptr)
		{
			if (std::isalnum(*ptr))
			{
				word_found = true;
				break;
			}
		}
		if (word_found)
		{
			size_t word_length = std::strlen(line);
			if (line[word_length - 1] == '\n')
			{
				--word_length;
			}
			m_devices.insert(std::string(line, word_length));
		}
	}
	::free(line);
	if (::feof(fo.m_file) == 0)
	{
		throw KrispDeviceListError("ERROR reading " + path + " file. " + std::strerror(errno) + ".");
	}
	fo.close();
}

bool DeviceList::add(const std::string & device)
{
	if (m_file_path.empty())
	{
		throw KrispDeviceListError("ERROR: no device list file is set.");
	}
	auto result = m_devices.insert(device);
	if (result.second)
	{
		try
		{
			append_device_to_file(device, m_file_path);
			// TODO: what if part of the device string is written to the file?
		}
		catch (const KrispDeviceListError & err)
		{
			m_devices.erase(device);
			throw err;
		}
		return true;
	}
	return false;
}

bool DeviceList::remove(const std::string & device)
{
	if (m_devices.erase(device) > 0)
	{
		if (m_file_path.empty())
		{
			throw KrispDeviceListError("ERROR: no device list file is set.");
		}
		try
		{
			write_device_set_to_file(m_devices, m_file_path);
		}
		catch(const KrispDeviceListError & err)
		{
			m_devices.insert(device);
			throw err;
		}
		return true;
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
