#include <cassert>
#include <memory>

#include <krisp-audio-sdk.hpp>

#include "krisp-exception.h"
#include "krisp-voice-sdk.h"
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

constexpr char allowListPath[] = PATH_JOIN(DEVICE_LISTS_DIR, "allowlist.txt");
constexpr char blockListPath[] = PATH_JOIN(DEVICE_LISTS_DIR, "blocklist.txt");
constexpr char badListPath[] = PATH_JOIN(DEVICE_LISTS_DIR, "badlist.txt");

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


TEST(InternalDeviceList, NoFileLoaded)
{
	using KrispVoiceSdk::KrispDeviceListError;
	KrispVoiceSdk::DeviceList dev_list;
	std::string device_1 = "Apple Air Pods Pro 2";
	EXPECT_THROW(dev_list.add(device_1), KrispDeviceListError);
	EXPECT_EQ(dev_list.count(), 0);
	EXPECT_FALSE(dev_list.remove(device_1));
	EXPECT_FALSE(dev_list.isInTheList(device_1));
}

constexpr char air_pods_pro_2[] = "  Apple Air Pods Pro 2 ";

TEST(InternalDeviceList, WithFileLoaded)
{
	{
		KrispVoiceSdk::DeviceList dev_list;
		EXPECT_NO_THROW(dev_list.createEmptyFile(blockListPath));
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
		KrispVoiceSdk::DeviceList dev_list;
		EXPECT_NO_THROW(dev_list.createEmptyFile(allowListPath));
		auto allow_devices = {"cheap headset", "any headset"};
		for (auto device : allow_devices)
		{
			EXPECT_TRUE(dev_list.add(device));
		}
		EXPECT_EQ(dev_list.count(), allow_devices.size());
	}
	KrispVoiceSdk::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	ASSERT_NO_THROW(dev_list.loadFromFile(blockListPath));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_FALSE(dev_list.add(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_TRUE(dev_list.isInTheList(device_in_the_list));
	EXPECT_TRUE(dev_list.remove(device_in_the_list));
	EXPECT_FALSE(dev_list.isInTheList(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 4);
	EXPECT_TRUE(dev_list.add(device_in_the_list));
	EXPECT_TRUE(dev_list.isInTheList(device_in_the_list));
	EXPECT_EQ(dev_list.count(), 5);
	EXPECT_NO_THROW(dev_list.loadFromFile(allowListPath));
	EXPECT_EQ(dev_list.count(), 2);
}

TEST(InternalDeviceList, BadList)
{
	KrispVoiceSdk::DeviceList dev_list;
	std::string device_in_the_list = air_pods_pro_2;
	EXPECT_NO_THROW(dev_list.loadFromFile(badListPath));
	EXPECT_EQ(dev_list.count(), 6);
}

class InternalTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		//KrispVoiceSDK::InitLibrary();
	}

	void TearDown() override
	{
		//KrispVoiceSDK::UnloadLibraryResources();
	}
};


TEST_F(InternalTest, Model)
{
	using KrispVoiceSdk::ModelId;
	KrispVoiceSdk::Model model;
	const char given_name[] = "any name";
	const char another_name[] = "another name";
	EXPECT_TRUE(model.load(model_nc_8k, ModelId::MicNc8K, given_name));
	EXPECT_TRUE(model.isLoaded());
	EXPECT_STREQ(model.getGivenName().c_str(), given_name);
	EXPECT_EQ(model.getLastError().size(), 0);
	EXPECT_FALSE(model.load(model_nc_16k, ModelId::MicNc16K, another_name));
	EXPECT_GT(model.getLastError().size(), 0);
	EXPECT_TRUE(model.hasError());
	EXPECT_GT(model.pullLastError().size(), 0);
	EXPECT_FALSE(model.hasError());
	EXPECT_TRUE(model.unload());
	EXPECT_FALSE(model.isLoaded());
	EXPECT_FALSE(model.hasError());
	EXPECT_TRUE(model.load(model_nc_16k, ModelId::MicNc16K, another_name));
	EXPECT_TRUE(model.isLoaded());
	EXPECT_STREQ(model.getGivenName().c_str(), another_name);
	EXPECT_FALSE(model.hasError());
	EXPECT_EQ(model.getLastError().size(), 0);
	EXPECT_TRUE(model.unload());
	EXPECT_FALSE(model.isLoaded());
}

TEST_F(InternalTest, ModelContainer)
{
	using KrispVoiceSdk::KrispException;
	using KrispVoiceSdk::ModelId;
	const unsigned model_count = 3;
	std::array<const wchar_t *, model_count> models_path =
		{model_nc_8k, model_nc_16k, model_nc_32k};
	KrispVoiceSdk::ModelContainer<model_count> container;
	EXPECT_EQ(container.getModelCount(), model_count);
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_FALSE(container.isModelRegistered(ModelId(id)));
	}
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_NO_THROW(container.registerModel(ModelId(id), models_path[id]));
	}
	EXPECT_THROW(container.registerModel(ModelId(3), model_bvc_32k), KrispException);
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_NO_THROW(container.isModelRegistered(ModelId(id)));
	}
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_NO_THROW(container.preloadModel(ModelId(id)));
	}
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		auto model_shared_ptr = container.getModel(ModelId(id));
		EXPECT_NE(model_shared_ptr.get(), nullptr);
		EXPECT_EQ(model_shared_ptr.use_count(), 2);
	}
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_NO_THROW(container.disableModelOwnership(ModelId(id)));
	}
	{
		std::array<std::shared_ptr<KrispVoiceSdk::Model>, model_count> models;
		for (unsigned id = 0; id < container.getModelCount(); ++id)
		{
			auto model_shared_ptr = container.getModel(ModelId(id));
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 1);
			models[id] = model_shared_ptr;
		}
		for (unsigned id = 0; id < container.getModelCount(); ++id)
		{
			auto model_shared_ptr = container.getModel(ModelId(id));
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 2);
		}
	}
	{
		for (unsigned id = 0; id < container.getModelCount(); ++id)
		{
			auto model_shared_ptr = container.getModel(ModelId(id));
			EXPECT_NE(model_shared_ptr.get(), nullptr);
			EXPECT_EQ(model_shared_ptr.use_count(), 1);
		}
	}
	for (unsigned id = 0; id < container.getModelCount(); ++id)
	{
		EXPECT_NO_THROW(container.enableModelOwnership(ModelId(id)));
	}
}

TEST_F(InternalTest, InternalAudioProcessorBuilder)
{
	KrispVoiceSdk::InternalAudioProcessorBuilder builder;
	using ModelId = KrispVoiceSdk::ModelId;
	using SamplingRate = KrispVoiceSdk::SamplingRate;
	EXPECT_NO_THROW(builder.accessBvcDeviceManager().loadLists(allowListPath, blockListPath));
	EXPECT_NO_THROW(builder.registerModel(ModelId::MicNc8K, model_nc_8k));
	EXPECT_NO_THROW(builder.registerModel(ModelId::MicNc16K, model_nc_16k));
	EXPECT_NO_THROW(builder.registerModel(ModelId::MicNc32K, model_nc_32k));
	EXPECT_NO_THROW(builder.registerModel(ModelId::MicBvc32K, model_bvc_32k));
	auto audio_cleaner_ptr = builder.createOutboundNoiseCleaner(SamplingRate::Sr8000);
	EXPECT_NE(audio_cleaner_ptr.get(), nullptr);
	auto audio_cleaner_2_ptr = builder.createOutboundNoiseCleaner(SamplingRate::Sr8000);
	EXPECT_NE(audio_cleaner_2_ptr.get(), nullptr);
	EXPECT_NE(audio_cleaner_ptr.get(), audio_cleaner_2_ptr.get());
	auto audio_cleaner_3_ptr = builder.createNoiseCleaner(SamplingRate::Sr16000, ModelId::MicNc16K);
	EXPECT_NE(audio_cleaner_3_ptr.get(), nullptr);
}

TEST(ClientCode, BvcDeviceLists)
{
	auto vBuilder = KrispVoiceSdk::VoiceProcessorBuilder();
	using KrispVoiceSdk::KrispDeviceListError;

	EXPECT_THROW(vBuilder.loadBvcDeviceLists("a", "b"), KrispDeviceListError);
	EXPECT_THROW(vBuilder.loadBvcDeviceLists(allowListPath, "b"), KrispDeviceListError);
	EXPECT_THROW(vBuilder.loadBvcDeviceLists("a", blockListPath), KrispDeviceListError);
	EXPECT_NO_THROW(vBuilder.loadBvcDeviceLists(allowListPath, blockListPath));
	EXPECT_NO_THROW(vBuilder.loadBvcDeviceLists(allowListPath, badListPath));
}

TEST(ClientCode, BvcDeviceListsNotSpecified)
{
	auto vBuilder = KrispVoiceSdk::VoiceProcessorBuilder();
	using KrispVoiceSdk::KrispDeviceListError;
	std::string device_1 = "Apple Air Pods Pro 2";
	EXPECT_THROW(vBuilder.allowBvcDevice(device_1), KrispDeviceListError);
	EXPECT_FALSE(vBuilder.removeBvcDevice(device_1));
	EXPECT_FALSE(vBuilder.isBvcAllowed(device_1));
	EXPECT_NO_THROW(vBuilder.forceBvc(true));
	EXPECT_TRUE(vBuilder.isBvcAllowed(device_1));
	EXPECT_NO_THROW(vBuilder.forceBvc(false));
	EXPECT_FALSE(vBuilder.isBvcAllowed(device_1));
}

TEST(ClientCode, BvcDeviceListsSpecified)
{
	{
		auto vBuilder = KrispVoiceSdk::VoiceProcessorBuilder();
		using KrispVoiceSdk::KrispDeviceListError;
		{
			KrispVoiceSdk::DeviceList allowList;
			EXPECT_NO_THROW(allowList.createEmptyFile(allowListPath));
		}
		auto blockDevices = {
			"super device 1",
			"super device 2",
			air_pods_pro_2,
			"Apple Air Pods Pro 1",
			"Another Fake device"
		};
		{
			KrispVoiceSdk::DeviceList blockList;
			EXPECT_NO_THROW(blockList.createEmptyFile(blockListPath));
			for (auto device : blockDevices)
			{
				EXPECT_TRUE(blockList.add(device));
			}
		}
		EXPECT_NO_THROW(vBuilder.loadBvcDeviceLists(allowListPath, blockListPath));
		for (auto device : blockDevices)
		{
			EXPECT_FALSE(vBuilder.isBvcAllowed(device));
			vBuilder.forceBvc(true);
			EXPECT_FALSE(vBuilder.isBvcAllowed(device));
			vBuilder.forceBvc(false);
			EXPECT_FALSE(vBuilder.isBvcAllowed(device));
		}
		auto allow_devices = {"cheap headset", "any headset"};
		for (auto device : allow_devices)
		{
			EXPECT_TRUE(vBuilder.allowBvcDevice(device));
			EXPECT_FALSE(vBuilder.allowBvcDevice(device));
		}
		{
			KrispVoiceSdk::DeviceList allowList;
			ASSERT_NO_THROW(allowList.loadFromFile(allowListPath));
			EXPECT_EQ(allowList.count(), allow_devices.size());
		}
	}
}

}
