#include <cassert>
#include <krisp-audio-sdk.hpp>

#include "device_list.h"
#include "gtest/gtest.h"

#include "outbound_session_factory.h"


namespace
{

#ifndef TEST_DATA_DIR
static_assert(0, "TEST_DATA_DIR macro should be set");
#endif

#ifdef PATH_JOIN
static_assert(0, "please rename conflicting macro");
#endif

#ifdef WIN32
#define PATH_JOIN(base, filename) base "\\" filename
#else
#define PATH_JOIN(base, filename) base "/" filename
#endif

constexpr char allow_list_path[] = PATH_JOIN(TEST_DATA_DIR, "allowlist.txt");
constexpr char block_list_path[] = PATH_JOIN(TEST_DATA_DIR, "blocklist.txt");
constexpr char bad_list_path[] = PATH_JOIN(TEST_DATA_DIR, "badlist.txt");

#undef PATH_JOIN


TEST(DeviceList, NoFileLoaded)
{
	ASSERT_TRUE(KrispAudioSDK::InitLibrary());

	KrispAudioSDK::DeviceList dev_list;
	std::string device_1 = "Apple Air Pods Pro 2";

	EXPECT_FALSE(dev_list.add(device_1));
	EXPECT_EQ(dev_list.count(), 0);
	EXPECT_TRUE(dev_list.has_error());
	EXPECT_NE(dev_list.pull_last_error().size(), 0);
	EXPECT_FALSE(dev_list.has_error());
	EXPECT_FALSE(dev_list.remove(device_1));
	EXPECT_FALSE(dev_list.has_error());
	EXPECT_FALSE(dev_list.is_in_the_list(device_1));
	EXPECT_FALSE(dev_list.has_error());

	ASSERT_TRUE(KrispAudioSDK::UnloadLibraryResources());
}


constexpr char air_pods_pro_2[] = "  Apple Air Pods Pro 2 ";

TEST(DeviceList, WithFileLoaded)
{
	ASSERT_TRUE(KrispAudioSDK::InitLibrary());
	{
		KrispAudioSDK::DeviceList dev_list;
		EXPECT_TRUE(dev_list.create_empty_file(block_list_path))
			<< dev_list.get_last_error();
		auto block_devices = {
			"super device 1",
			"super device 2",
			air_pods_pro_2,
			"Apple Air Pods Pro 1",
			"Another Fake device"
		};
		for (auto device : block_devices)
		{
			EXPECT_TRUE(dev_list.add(device));
		}
		EXPECT_FALSE(dev_list.has_error());
		EXPECT_EQ(dev_list.count(), block_devices.size());
	}
	{
		KrispAudioSDK::DeviceList dev_list;
		EXPECT_TRUE(dev_list.create_empty_file(allow_list_path))
			<< dev_list.get_last_error();
		auto allow_devices = {"cheap headset", "any headset"};
		for (auto device : allow_devices)
		{
			EXPECT_TRUE(dev_list.add(device));
		}
		EXPECT_FALSE(dev_list.has_error());
		EXPECT_EQ(dev_list.count(), allow_devices.size());
	}
	KrispAudioSDK::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	ASSERT_TRUE(dev_list.load_from_file(block_list_path))
		<< dev_list.get_last_error();
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_FALSE(dev_list.add(device_in_the_list));
	EXPECT_FALSE(dev_list.has_error());
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_TRUE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_TRUE(dev_list.remove(device_in_the_list));
	EXPECT_FALSE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 4);
	EXPECT_FALSE(dev_list.has_error());
	EXPECT_TRUE(dev_list.add(device_in_the_list));
	EXPECT_TRUE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_FALSE(dev_list.has_error());
	EXPECT_TRUE(dev_list.load_from_file(allow_list_path));
	EXPECT_EQ(dev_list.count(), 2);
	ASSERT_TRUE(KrispAudioSDK::UnloadLibraryResources());
}

TEST(DeviceList, BadList)
{
	ASSERT_TRUE(KrispAudioSDK::InitLibrary());
	KrispAudioSDK::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	EXPECT_TRUE(dev_list.load_from_file(bad_list_path)) 
		<< dev_list.get_last_error();
	//for (auto d : dev_list.get_container_ref())
	//{
	//}
	EXPECT_EQ(dev_list.count(), 6);
	ASSERT_TRUE(KrispAudioSDK::UnloadLibraryResources());
}

}
