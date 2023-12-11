#pragma once

#include <memory>
#include <string>

#include "bvc_device_manager.h"
#include "krisp-voice-sdk.h"
#include "library_resources.h"
#include "model.h"
#include "model_container.h"

namespace KrispVoiceSDK
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
     * You must call unregisterModel before using the blobAddr memory block
     * for something else.
     */
    void registerModel(ModelId id, void* blobAddr, size_t blobSize);
    /**
     * Let the system know that the model is available on the filesystem.
     * You must call unregisterModel if the file will not be available for any
     * reason.
     */
    void registerModel(ModelId id, const std::wstring& path);
    /**
     * Let the system know that the model previusly registered is not available
     * anymore.
     */
    void unregisterModel(ModelId id);
    /**
     * Set the memory policy for the model.
     */
    void setModelPolicy(ModelId id, ModelMemoryPolicy policyId);
    /**
     * Load the model into the memory. Otherwise the model will be loaded
     * runtime during FrameCleaner creation which may introduce latency
     * depending on the model size and the CPU.
     */
    void preloadModel(ModelId id);

    std::unique_ptr<AudioProcessor> createBvc(SamplingRate, const std::string& device);
    std::unique_ptr<AudioProcessor> createNc(SamplingRate);
    std::unique_ptr<AudioProcessor> createNc(SamplingRate, ModelId modelId);

    BVCDeviceManager& accessBvcDeviceManager();

private:
    ModelContainer<ModelsNumber> _modelContainer;
    BVCDeviceManager _deviceManager;
    std::shared_ptr<LibraryResources> _libraryPtr;

    std::shared_ptr<Model> chooseModel8K();
    std::shared_ptr<Model> chooseModel16K();
    std::shared_ptr<Model> chooseModel(const std::string& device, SamplingRate r, bool tryBvc);
    std::shared_ptr<Model> getModel(ModelId);
};

} // namespace KrispVoiceSDK
