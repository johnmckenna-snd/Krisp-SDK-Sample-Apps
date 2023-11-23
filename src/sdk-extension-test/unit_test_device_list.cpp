#include <cassert>
#include <memory>

#include <krisp-audio-sdk.hpp>

#include "krisp-exception.h"
#include "krisp-audio-sdk-ext.h"
#include "audio-processor.h"
#include "device_list.h"
#include "gtest/gtest.h"

#include "model_container.h"
#include "audio_processor_builder.h"
#include "model_container_impl.h"


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

#ifdef DEVICE_LISTS_DIR
static_assert(0, "please rename conflicting macro");
#endif
#define DEVICE_LISTS_DIR PATH_JOIN(TEST_DATA_DIR, "device-lists")

constexpr char allow_list_path[] = PATH_JOIN(DEVICE_LISTS_DIR, "allowlist.txt");
constexpr char block_list_path[] = PATH_JOIN(DEVICE_LISTS_DIR, "blocklist.txt");
constexpr char bad_list_path[] = PATH_JOIN(DEVICE_LISTS_DIR, "badlist.txt");

#undef DEVICE_LISTS_DIR

#ifdef MODELS_DIR
static_assert(0, "please rename conflicting macro");
#endif
#define MODELS_DIR PATH_JOIN(TEST_DATA_DIR, "models")

constexpr wchar_t model_nc_8k[] = L"" PATH_JOIN(MODELS_DIR, "model-nc-8k.kw");
constexpr wchar_t model_nc_16k[] = L"" PATH_JOIN(MODELS_DIR, "model-nc-16k.kw");
constexpr wchar_t model_nc_32k[] = L"" PATH_JOIN(MODELS_DIR, "model-nc-32k.kw");
constexpr wchar_t model_bvc_32k[] = L"" PATH_JOIN(MODELS_DIR, "model-bvc-32k.kw");

#undef MODELS_DIR
#undef PATH_JOIN


TEST(DeviceList, NoFileLoaded)
{
	using KrispAudioSDK::KrispDeviceListError;
	KrispAudioSDK::DeviceList dev_list;
	std::string device_1 = "Apple Air Pods Pro 2";
	EXPECT_THROW(dev_list.add(device_1), KrispDeviceListError);
	EXPECT_EQ(dev_list.count(), 0);
	EXPECT_FALSE(dev_list.remove(device_1));
	EXPECT_FALSE(dev_list.is_in_the_list(device_1));
}

constexpr char air_pods_pro_2[] = "  Apple Air Pods Pro 2 ";

TEST(DeviceList, WithFileLoaded)
{
	{
		KrispAudioSDK::DeviceList dev_list;
		EXPECT_NO_THROW(dev_list.create_empty_file(block_list_path));
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
		EXPECT_EQ(dev_list.count(), block_devices.size());
	}
	{
		KrispAudioSDK::DeviceList dev_list;
		EXPECT_NO_THROW(dev_list.create_empty_file(allow_list_path));
		auto allow_devices = {"cheap headset", "any headset"};
		for (auto device : allow_devices)
		{
			EXPECT_TRUE(dev_list.add(device));
		}
		EXPECT_EQ(dev_list.count(), allow_devices.size());
	}
	KrispAudioSDK::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	ASSERT_NO_THROW(dev_list.load_from_file(block_list_path));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_FALSE(dev_list.add(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_TRUE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_TRUE(dev_list.remove(device_in_the_list));
	EXPECT_FALSE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 4);
	EXPECT_TRUE(dev_list.add(device_in_the_list));
	EXPECT_TRUE(dev_list.is_in_the_list(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_NO_THROW(dev_list.load_from_file(allow_list_path));
	EXPECT_EQ(dev_list.count(), 2);
}

TEST(DeviceList, BadList)
{
	KrispAudioSDK::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	EXPECT_NO_THROW(dev_list.load_from_file(bad_list_path));
	EXPECT_EQ(dev_list.count(), 6);
}

class Test : public ::testing::Test
{
protected:
	void SetUp() override
	{
		//KrispAudioSDK::InitLibrary();
	}

	void TearDown() override
	{
		//KrispAudioSDK::UnloadLibraryResources();
	}
};


TEST_F(Test, Model)
{
	KrispAudioSDK::Model model;
	const char given_name[] = "any name";
	const char another_name[] = "another name";
	EXPECT_TRUE(model.load(model_nc_8k, given_name));
	EXPECT_TRUE(model.is_loaded());
	EXPECT_STREQ(model.get_given_name().c_str(), given_name);
	EXPECT_EQ(model.get_last_error().size(), 0);
	EXPECT_FALSE(model.load(model_nc_16k, another_name));
	EXPECT_GT(model.get_last_error().size(), 0);
	EXPECT_TRUE(model.has_error());
	EXPECT_GT(model.pull_last_error().size(), 0);
	EXPECT_FALSE(model.has_error());
	EXPECT_TRUE(model.unload());
	EXPECT_FALSE(model.is_loaded());
	EXPECT_FALSE(model.has_error());
	EXPECT_TRUE(model.load(model_nc_16k, another_name));
	EXPECT_TRUE(model.is_loaded());
	EXPECT_STREQ(model.get_given_name().c_str(), another_name);
	EXPECT_FALSE(model.has_error());
	EXPECT_EQ(model.get_last_error().size(), 0);
	EXPECT_TRUE(model.unload());
	EXPECT_FALSE(model.is_loaded());
}

TEST_F(Test, ModelContainer)
{
	using KrispAudioSDK::KrispException;
	const unsigned model_count = 3;
	std::array<const wchar_t *, model_count> models_path =
		{model_nc_8k, model_nc_16k, model_nc_32k};
	KrispAudioSDK::ModelContainer<model_count> container;
	EXPECT_EQ(container.get_model_count(), model_count);
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_FALSE(container.is_model_registered(id));
	}
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_NO_THROW(container.register_model(id, models_path[id]));
	}
	EXPECT_THROW(container.register_model(3, model_bvc_32k), KrispException);
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_NO_THROW(container.is_model_registered(id));
	}
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_NO_THROW(container.preload_model(id));
	}
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		auto model_shared_ptr = container.get_model(id);
		EXPECT_NE(model_shared_ptr.get(), nullptr);
		EXPECT_EQ(model_shared_ptr.use_count(), 2);
	}
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_NO_THROW(container.disable_model_ownership(id));
	}
	{
		std::array<std::shared_ptr<KrispAudioSDK::Model>, model_count> models;
		for (unsigned id = 0; id < container.get_model_count(); ++id)
		{
			auto model_shared_ptr = container.get_model(id);
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 1);
			models[id] = model_shared_ptr;
		}
		for (unsigned id = 0; id < container.get_model_count(); ++id)
		{
			auto model_shared_ptr = container.get_model(id);
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 2);
		}
	}
	{
		for (unsigned id = 0; id < container.get_model_count(); ++id)
		{
			auto model_shared_ptr = container.get_model(id);
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 1);
		}
	}
	for (unsigned id = 0; id < container.get_model_count(); ++id)
	{
		EXPECT_NO_THROW(container.enable_model_ownership(id));
	}
}

TEST_F(Test, AudioProcessorBuilder)
{
	KrispAudioSDK::AudioProcessorBuilder builder;
	using ModelId = KrispAudioSDK::ModelId;
	using SamplingRate = KrispAudioSDK::SamplingRate;
	EXPECT_NO_THROW(builder.load_device_lists(allow_list_path, block_list_path));
	EXPECT_NO_THROW(builder.register_model(ModelId::mic_nc_8k, model_nc_8k));
	EXPECT_NO_THROW(builder.register_model(ModelId::mic_nc_16k, model_nc_16k));
	EXPECT_NO_THROW(builder.register_model(ModelId::mic_nc_32k, model_nc_32k));
	EXPECT_NO_THROW(builder.register_model(ModelId::mic_bvc_32k, model_bvc_32k));
	auto audio_cleaner_ptr = builder.create_nc(SamplingRate::sampling_rate_8000);
	EXPECT_NE(audio_cleaner_ptr.get(), nullptr);
	auto audio_cleaner_2_ptr = builder.create_nc(SamplingRate::sampling_rate_8000);
	EXPECT_NE(audio_cleaner_2_ptr.get(), nullptr);
	EXPECT_NE(audio_cleaner_ptr.get(), audio_cleaner_2_ptr.get());
}

}
