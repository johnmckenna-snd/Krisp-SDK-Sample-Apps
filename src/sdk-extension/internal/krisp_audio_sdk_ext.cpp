#include "krisp-audio-sdk-ext.h"

#include "audio_processor_builder.h"
#include "model_scanner.h"


namespace KrispAudioSDK
{

AudioProcessorBuilder * get_builder()
{
    static AudioProcessorBuilder frame_cleaner_builder;
    return &frame_cleaner_builder;
}

std::vector<ModelId> register_models_in_directory(const std::string & model_dir,
    const ModelNameToIdMap & model_name_to_id_map)
{
    return get_models_from_directory(*get_builder(), model_dir, model_name_to_id_map);
}

void register_model(ModelId id, const std::wstring & path)
{
    get_builder()->register_model(id, path);
}

void register_model(ModelId id, void * blob_ptr, size_t blob_size)
{
    get_builder()->register_model(id, blob_ptr, blob_size);
}

void unregister_model(ModelId id)
{
    get_builder()->unregister_model(id);
}

void preload_model(ModelId id)
{
    get_builder()->preload_model(id);
}

void set_model_policy(ModelId id, ModelMemoryPolicy policy)
{
    get_builder()->set_model_policy(id, policy);
}

void load_bvc_device_lists(const std::string & allow_list_path, const std::string & block_list_path)
{
    get_builder()->load_device_lists(allow_list_path, block_list_path);
}

std::unique_ptr<AudioProcessor> create_nc(SamplingRate rate)
{
    return get_builder()->create_nc(rate);
}

std::unique_ptr<AudioProcessor> create_bvc(SamplingRate rate, const std::string & device)
{
    return get_builder()->create_bvc(rate, device);
}


}
