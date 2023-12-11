#include "krisp-voice-sdk.h"

#include "audio_processor_builder.h"
#include "model_scanner.h"

namespace KrispVoiceSDK
{

AudioProcessorBuilder* builder_ptr = nullptr;

AudioProcessorBuilder* getBuilder()
{
    if (!builder_ptr)
    {
        builder_ptr = new AudioProcessorBuilder;
    }
    return builder_ptr;
}

void freeLibrary()
{
    delete builder_ptr;
    builder_ptr = nullptr;
}

std::vector<ModelId> registerModelsDirectory(
    const std::string& model_dir, const ModelNameToIdMap& model_name_to_id_map)
{
    return GetModelsFromDirectory(
        *getBuilder(), model_dir, model_name_to_id_map);
}

void registerModel(ModelId id, const std::wstring& path)
{
    getBuilder()->registerModel(id, path);
}

void registerModel(ModelId id, void* blob_ptr, size_t blob_size)
{
    getBuilder()->registerModel(id, blob_ptr, blob_size);
}

void unregisterModel(ModelId id)
{
    getBuilder()->unregisterModel(id);
}

void preloadModel(ModelId id)
{
    getBuilder()->preloadModel(id);
}

void setModelPolicy(ModelId id, ModelMemoryPolicy policy)
{
    getBuilder()->setModelPolicy(id, policy);
}

void loadBvcDeviceLists(
    const std::string& allow_list_path, const std::string& block_list_path)
{
    getBuilder()->accessBvcDeviceManager().loadLists(
        allow_list_path, block_list_path);
}

std::unique_ptr<AudioProcessor> createNc(SamplingRate rate)
{
    return getBuilder()->createNc(rate);
}

std::unique_ptr<AudioProcessor> createNc(SamplingRate rate, ModelId model_id)
{
    return getBuilder()->createNc(rate);
}

std::unique_ptr<AudioProcessor> createBvc(
    SamplingRate rate, const std::string& device)
{
    return getBuilder()->createBvc(rate, device);
}

bool allowBvcDevice(const std::string& deviceName)
{
    return getBuilder()->accessBvcDeviceManager().allowDevice(deviceName);
}

bool blockBvcDevice(const std::string& deviceName)
{
    return getBuilder()->accessBvcDeviceManager().blockDevice(deviceName);
}

bool removeBvcDevice(const std::string& deviceName)
{
    return getBuilder()->accessBvcDeviceManager().removeDevice(deviceName);
}

bool isBvcAllowed(const std::string& deviceName)
{
    return getBuilder()->accessBvcDeviceManager().isAllowed(deviceName);
}

void forceBvc()
{
    getBuilder()->accessBvcDeviceManager().forceBvc(true);
}

} // namespace KrispVoiceSDK
