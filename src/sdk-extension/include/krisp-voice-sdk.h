#ifndef KRISP_VOICE_SDK_H
#define KRISP_VOICE_SDK_H

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "krisp-audio-processor.h"
#include "krisp-enums.h"

namespace KrispVoiceSDK
{

typedef std::initializer_list<std::pair<const char*, ModelId>> ModelNameToIdMap;

constexpr ModelNameToIdMap modelMap = {
    {"model-nc-8.kw", ModelId::MicNc8K},
    {"model-nc-16.kw", ModelId::MicNc16K},
    {"model-nc-32.kw", ModelId::MicNc32K},
    {"model-bvc-32.thw", ModelId::MicBvc32K},
    {"c5.n.s.20949d.kw", ModelId::MicNc8K},
    {"c5.s.w.c9ac8f.kw", ModelId::MicNc16K},
    {"c6.f.s.ced125.kw", ModelId::MicNc32K},
    {"hs.c6.f.s.de56df.thw", ModelId::MicBvc32K}};

// Scan the directory for known SDK models and register all of them to the system. The function
// will call registerModel for each identified model in the directory.
std::vector<ModelId> registerModelsDirectory(const std::string& path, const ModelNameToIdMap& nameToId = modelMap);

/**
 * The memory policy indicates if the model should be kept in the memory for future use if not
 * currently needed. Or the memory should be released if the model is not used.
 */
enum class ModelMemoryPolicy
{
    KeepCachedAfterLoad = 0,
    UnloadIfNotUsed = 1
};

class AudioProcessor;

// Let the system know about the that model exists and available on the specified location. The
// system model will be used on demand or if cached in advance.
void registerModel(ModelId id, const std::wstring& path);

// Let the system know that the model is available in the memory.
void registerModel(ModelId id, void* blobPtr, size_t blobSize);

// Let the system know that the model should not be used anymore for new noise cancellers
void unregisterModel(ModelId id);

// Load the model into the memory in advance before the need.
void preloadModel(ModelId id);

// Configure how model resources should be handled. Two policies are available.
// Always keep cached or release if not used.
void setModelPolicy(ModelId id, ModelMemoryPolicy policy);

// Create noise cleaner with model auto selection
std::unique_ptr<AudioProcessor> createNc(SamplingRate r);

// Create noise cleaner using the specified model
std::unique_ptr<AudioProcessor> createNc(SamplingRate r, ModelId modelId);

// Create noise cleaner with BVC if device is supported, otherwise NC model is used
std::unique_ptr<AudioProcessor> createBvc(SamplingRate r, const std::string& device);

// Create noise cleaner with model auto selection with per frame and total noise stats support
std::unique_ptr<AudioNoiseCleanerWithStats> createNcWithStats(SamplingRate);

// Create noise cleaner using the specified model with per frame and total noise stats support
std::unique_ptr<AudioNoiseCleanerWithStats> createNcWithStats(SamplingRate, ModelId modelId);

// Create noise cleaner with noise stats support with BVC if device is supported, otherwise NC
// model is used
std::unique_ptr<AudioNoiseCleanerWithStats> createBvcWithStats(SamplingRate, const std::string&);

// std::unique_ptr<AudioAccentProcessor> createAccentProcessor(SamplingRate r);

// release resources if the library is not used
void freeLibrary();

// Load BVC allow list and block list to the system. The lists will be used to consider if BVC
// model can be used.
void loadBvcDeviceLists(const std::string& allowListPath, const std::string& blockListPath);

bool allowBvcDevice(const std::string & deviceName);
bool blockBvcDevice(const std::string & deviceName);
bool removeBvcDevice(const std::string & deviceName);
bool isBvcAllowed(const std::string & deviceName);
void forceBvc();

} // namespace KrispVoiceSDK

#endif
