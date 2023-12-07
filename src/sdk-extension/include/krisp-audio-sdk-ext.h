#pragma once

#include <initializer_list>
#include <string>
#include <vector>
#include <memory>

#include "krisp-enums.h"
#include "krisp-audio-processor.h"


namespace KrispVoiceSDK
{

typedef std::initializer_list<std::pair<const char *, ModelId>> ModelNameToIdMap;

constexpr ModelNameToIdMap modelMap =
{
    {"model-nc-8.kw", ModelId::MicNc8K},
    {"model-nc-16.kw", ModelId::MicNc16K},
    {"model-nc-32.kw", ModelId::MicNc32K},
    {"model-bvc-32.thw", ModelId::MicBvc32K},
    {"c5.n.s.20949d.kw", ModelId::MicNc8K},
    {"c5.s.w.c9ac8f.kw", ModelId::MicNc16K},
    {"c6.f.s.ced125.kw", ModelId::MicNc32K},
    {"hs.c6.f.s.de56df.thw", ModelId::MicBvc32K}
};

/**
 * The memory policy indicates if the model should be kept in the memory for future use if not
 * currently needed. Or the memory should be released if the model is not used.
 */
enum ModelMemoryPolicy
{
	KeepCachedAfterLoad = 0,
	UnloadIfNotUsed = 1
};

class AudioProcessor;


std::vector<ModelId> registerModelsDirectory(const std::string & path,
    const ModelNameToIdMap & nameToId = modelMap);

void registerModel(ModelId id, const std::wstring & path);
void registerModel(ModelId id, void * blobPtr, size_t blobSize);
void unregisterModel(ModelId id);
void preloadModel(ModelId id);
void setModelPolicy(ModelId id, ModelMemoryPolicy policy);
void loadBvcDeviceLists(const std::string &, const std::string &);

std::unique_ptr<AudioProcessor> createNc(SamplingRate r);
std::unique_ptr<AudioProcessor> createNc(SamplingRate r, ModelId modelId);
std::unique_ptr<AudioProcessor> createBvc(SamplingRate r, const std::string & device);
//std::unique_ptr<AudioProcessor> createNcWithStats(SamplingRate);
//std::unique_ptr<AudioProcessor> createBvcWithStats(SamplingRate, const std::string &);

}
