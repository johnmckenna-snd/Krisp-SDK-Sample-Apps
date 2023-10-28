#pragma once

#include "frame_processor_factory.h"

#include <string>
#include <memory>

#include "model.h"
#include "model_container.h"
#include "bvc_device_manager.h"


namespace KrispAudioSDK
{

bool InitLibrary();
bool UnloadLibraryResources();


/**
 * The class is responsible to provide FrameCleaner object for the given audio
 * stream. The user should register a list of AI models to the class. The class
 * will use the optimum model for the given audio stream considering the 
 * sampling rate and the device. The user should also provide BVC device allow
 * list and block list. The data will be used to select the optimum model 
 * for the audio stream and the device.
 */
class MicFrameCleanerFactory : public FrameProcessorFactory
{
public:
	/**
	 * The identifier of the AI model. The class knows only these 4 models.
	 */
	enum ModelId
	{
		nc_8k = 0,
		nc_16k = 1,
		nc_32k = 2,
		bvc_32k = 3
	};
	/**
	 * Each register model has model policy. It indicates if the model should
	 * be kept in the memory for future use if not currently needed. Or the
	 * memory should be released if the model is not used.
	 */
	enum ModelMemoryPolicy
	{
		keep_cached_after_load = 0,
		unload_if_not_used = 1
	};
	/**
	 * Let the system know that the model is available in the memory.
	 * You must call unregister_model before using the blob_ptr memory block
	 * for something else.
	 */
	bool register_model(ModelId id, void * blob_ptr, size_t blob_size);
	/**
	 * Let the system know that the model is available on the filesystem. 
	 * You must call unregister_model if the file will not be available for any
	 * reason.
	 */
	bool register_model(ModelId id, const std::wstring & path);
	/** 
	 * Let the system know that the model previusly registered is not available
	 * anymore.
	 */
	bool unregister_model(ModelId id);
	/**
	 * Set the memory policy for the model.
	 */
	bool set_model_policy(ModelId id, ModelMemoryPolicy policy_id);
	/**
	 * Load the model into the memory. Otherwise the model will be loaded
	 * runtime during FrameCleaner creation which may introduce latency
	 * depending on the model size and the CPU.
	 */
	bool preload_model(ModelId id);
	/**
	 * load BVC device allow and block list.
	 */
	bool load_device_lists(const std::string & allow_list_path,
		const std::string & block_list_path);
	/**
	 * Get the last error string. Empty string is an indicator that there is no
	 * error.
	 */
	const std::string & get_last_error() const;

	bool has_error() const;
private:
	ModelContainer<4> m_model_container;
	BVCDeviceManager m_device_manager;
	std::string m_last_error;
	std::unique_ptr<FrameProcessor> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc);
	std::shared_ptr<Model> choose_model(
		const std::string & device, SamplingRate r, bool try_bvc);
};

}
