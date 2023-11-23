#pragma once

#include <string>
#include <memory>

#include "krisp-audio-sdk-ext.h"
#include "library_resources.h"
#include "model.h"
#include "model_container.h"
#include "bvc_device_manager.h"


namespace KrispAudioSDK
{


/**
 * The class is responsible to provide FrameCleaner object for the given audio
 * stream. The user should register a list of AI models to the class. The class
 * will use the optimum model for the given audio stream considering the
 * sampling rate and the device. The user should also provide BVC device allow
 * list and block list. The data will be used to select the optimum model
 * for the audio stream and the device.
 */
class AudioProcessorBuilder
{
public:
	static constexpr unsigned long ModelsNumber = 6;
	AudioProcessorBuilder();
	/**
	 * Let the system know that the model is available in the memory.
	 * You must call unregister_model before using the blob_ptr memory block
	 * for something else.
	 */
	void register_model(ModelId id, void * blob_ptr, size_t blob_size);
	/**
	 * Let the system know that the model is available on the filesystem.
	 * You must call unregister_model if the file will not be available for any
	 * reason.
	 */
	void register_model(ModelId id, const std::wstring & path);
	/**
	 * Let the system know that the model previusly registered is not available
	 * anymore.
	 */
	void unregister_model(ModelId id);
	/**
	 * Set the memory policy for the model.
	 */
	void set_model_policy(ModelId id, ModelMemoryPolicy policy_id);
	/**
	 * Load the model into the memory. Otherwise the model will be loaded
	 * runtime during FrameCleaner creation which may introduce latency
	 * depending on the model size and the CPU.
	 */
	void preload_model(ModelId id);
	/**
	 * load BVC device allow and block list.
	 */
	void load_device_lists(const std::string & allow_list_path,
		const std::string & block_list_path);

	std::unique_ptr<AudioProcessor> create_bvc(SamplingRate, const std::string & device);
	std::unique_ptr<AudioProcessor> create_nc(SamplingRate);

private:
	ModelContainer<ModelsNumber> m_model_container;
	BVCDeviceManager m_device_manager;
	std::shared_ptr<LibraryResources> m_library_ptr;

	std::shared_ptr<Model> choose_model_8k();
	std::shared_ptr<Model> choose_model_16k();
	std::shared_ptr<Model> choose_model(
		const std::string & device, SamplingRate r, bool try_bvc);
	std::shared_ptr<Model> get_model(ModelId);
};

}
