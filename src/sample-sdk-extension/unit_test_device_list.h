#pragma once

#include <cassert>
#include "device_list.h"

using KrispAudioSDK::DeviceList;


namespace unit_test_device_list {


bool test_create_device_list(
	const std::initializer_list<const char *> & devices,
	const std::string & file_path)
{
	bool fail {false};
	DeviceList dev_list;
	if (!dev_list.create_empty_file(file_path))
	{
		fail = true;
		std::cerr << dev_list.get_last_error() << std::endl;
		assert(0);
	}
	for (auto device : devices)
	{
		if (dev_list.add(device) == false)
		{
			fail = true;
			assert(0);
		}
	}
	if (dev_list.has_error())
	{
		fail = true;
		assert(0);
	}
	if (dev_list.count() != devices.size())
	{
		fail = true;
		assert(0);
	}
	return !fail;
}

bool test_no_file_loaded()
{
	bool fail {false};
	DeviceList dev_list;

	std::string device_1 = "Apple Air Pods Pro 2";

	if (dev_list.add(device_1) != false) {
		// ERROR msg
		fail = true;
		assert(0);
	}

	if (dev_list.count() != 0) {
		// ERROR 
		fail = true;
		assert(0);
	}

	if (dev_list.has_error() == false) {
		// ERROR message should be set
		fail = true;
		assert(0);
	}

	if (!dev_list.pull_last_error().size()) {
		// ERROR message should be set
		fail = true;
		assert(0);
	}

	if (dev_list.has_error() == true) {
		// no ERROR msg should be stored after pull_last_error
		fail = true;
		assert(0);
	}

	if (dev_list.remove("Apple Air pods Pro 2") != false) {
		// explain
		fail = true;
		assert(0);
	}

	if (dev_list.has_error() == true) {
		// no error should be set
		fail = true;
		assert(0);
	}

	if (dev_list.is_in_the_list(device_1) != false) {
		fail = true;
		assert(0);
	}

	if (dev_list.has_error() == true) {
		// no error should be set
		fail = true;
		assert(0);
	}

	return !fail;
}

bool test_with_file_loaded()
{
	bool fail {false};
	auto block_devices = {
		"super device 1",
		"super device 2",
		"Apple Air Pods Pro 2",
		"Apple Air Pods Pro 1",
		"Another Fake device"
	};
	if (!test_create_device_list(block_devices, "blocklist.txt"))
	{
		fail = true;
		assert(0);
	}
	auto allow_devices = {"cheap headset", "any headset"};
	if (!test_create_device_list(allow_devices, "allowlist.txt"))
	{
		fail = true;
		assert(0);
	}

	DeviceList dev_list;

	std::string device_in_the_list = "Apple Air Pods Pro 2";

	if (dev_list.load_from_file("blocklist.txt") == false)
	{
		std::cout << dev_list.get_last_error() << std::endl;
		fail = true;
		assert(0);
	}
	if (dev_list.count() != 5)
	{
		std::cout << dev_list.count() << std::endl;
		fail = true;
		assert(0);
	}
	assert(0);
	if (dev_list.add(device_in_the_list) == true)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.has_error() == true)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.count() != 5)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.is_in_the_list(device_in_the_list) == false)
	{
		fail = true;
		assert(0);
	}
	assert(0);
	if (dev_list.remove(device_in_the_list) == false)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.is_in_the_list(device_in_the_list) == true)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.count() != 4)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.has_error() == true)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.add(device_in_the_list) == false)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.is_in_the_list(device_in_the_list) == true)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.count() != 5)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.has_error() == true)
	{
		fail = true;
		assert(0);
	}
	assert(0);
	if (dev_list.load_from_file("allowlist.txt") == false)
	{
		fail = true;
		assert(0);
	}
	if (dev_list.count() != 2)
	{
		fail = true;
		assert(0);
	}
	assert(0);
	return !fail;
}


}
