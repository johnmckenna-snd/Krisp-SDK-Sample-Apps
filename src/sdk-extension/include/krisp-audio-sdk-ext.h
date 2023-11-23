#pragma once

#include <initializer_list>
#include <string>
#include <vector>
#include <memory>

#include "audio-processor.h"


namespace KrispAudioSDK
{

enum ModelId
{
    mic_nc_8k = 0,
    mic_nc_16k = 1,
    mic_nc_32k = 2,
    mic_bvc_32k = 3,
    speaker_nc_8k = 4,
    speaker_nc_16k = 5,
	undefined = -1
};

enum SamplingRate
{
	sampling_rate_8000 = 8000,
	sampling_rate_16000 = 16000,
	sampling_rate_32000 = 32000,
	sampling_rate_44100 = 44100,
	sampling_rate_48000 = 48000,
	sampling_rate_88200 = 88200,
	sampling_rate_96000 = 96000
};

typedef std::initializer_list<std::pair<const char *, ModelId>> ModelNameToIdMap;

constexpr ModelNameToIdMap modelMap =
{
    {"model-nc-8.kw", ModelId::mic_nc_8k},
    {"model-nc-16.kw", ModelId::mic_nc_16k},
    {"model-nc-32.kw", ModelId::mic_nc_32k},
    {"model-bvc-32.thw", ModelId::mic_bvc_32k},
    {"c5.n.s.20949d.kw", ModelId::mic_nc_8k},
    {"c5.s.w.c9ac8f.kw", ModelId::mic_nc_16k},
    {"c6.f.s.ced125.kw", ModelId::mic_nc_32k},
    {"hs.c6.f.s.de56df.thw", ModelId::mic_bvc_32k}
};

/**
 * The memory policy indicates if the model should be kept in the memory for future use if not
 * currently needed. Or the memory should be released if the model is not used.
 */
enum ModelMemoryPolicy
{
	keep_cached_after_load = 0,
	unload_if_not_used = 1
};

class AudioProcessor;


std::vector<ModelId> register_models_in_directory(const std::string & path,
    const ModelNameToIdMap & name_to_id = modelMap);

void register_model(ModelId id, const std::wstring & path);
void register_model(ModelId id, void * blob_ptr, size_t blob_size);
void unregister_model(ModelId id);
void preload_model(ModelId id);
void set_model_policy(ModelId id, ModelMemoryPolicy policy);
void load_bvc_device_lists(const std::string &, const std::string &);

std::unique_ptr<AudioProcessor> create_nc(SamplingRate r);
std::unique_ptr<AudioProcessor> create_bvc(SamplingRate r, const std::string & device);
//std::unique_ptr<AudioProcessor> create_nc_stats_processor(SamplingRate);
//std::unique_ptr<AudioProcessor> create_nc_bvc_stats_processor(SamplingRate, const std::string &);

}
