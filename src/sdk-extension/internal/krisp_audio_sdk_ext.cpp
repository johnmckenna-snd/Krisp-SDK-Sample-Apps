#include "krisp-audio-sdk-ext.h"

#include "audio_processor_builder.h"
#include "model_scanner.h"


namespace KrispVoiceSDK
{

AudioProcessorBuilder * get_builder()
{
    static AudioProcessorBuilder frame_cleaner_builder;
    return &frame_cleaner_builder;
}

std::vector<ModelId> registerModelsDirectory(const std::string & model_dir,
    const ModelNameToIdMap & model_name_to_id_map)
{
    return get_models_from_directory(*get_builder(), model_dir, model_name_to_id_map);
}

void registerModel(ModelId id, const std::wstring & path)
{
    get_builder()->registerModel(id, path);
}

void registerModel(ModelId id, void * blob_ptr, size_t blob_size)
{
    get_builder()->registerModel(id, blob_ptr, blob_size);
}

void unregisterModel(ModelId id)
{
    get_builder()->unregisterModel(id);
}

void preloadModel(ModelId id)
{
    get_builder()->preloadModel(id);
}

void setModelPolicy(ModelId id, ModelMemoryPolicy policy)
{
    get_builder()->setModelPolicy(id, policy);
}

void loadBvcDeviceLists(const std::string & allow_list_path, const std::string & block_list_path)
{
    get_builder()->load_device_lists(allow_list_path, block_list_path);
}

std::unique_ptr<AudioProcessor> createNc(SamplingRate rate)
{
    return get_builder()->createNc(rate);
}

std::unique_ptr<AudioProcessor> createNc(SamplingRate rate, ModelId model_id)
{
    return get_builder()->createNc(rate);
}

std::unique_ptr<AudioProcessor> createBvc(SamplingRate rate, const std::string & device)
{
    return get_builder()->createBvc(rate, device);
}


}
