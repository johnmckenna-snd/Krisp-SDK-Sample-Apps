#include "krisp-voice-sdk.h"

#include <memory>

#include "audio_processor_builder.h"
#include "model_scanner.h"

namespace KrispVoiceSdk
{

NoiseCleaner::NoiseCleaner(ModelId model_id) : _modelId(model_id)
{
}

NoiseCleaner::~NoiseCleaner()
{
}

NoiseCleaner::NoiseCleaner(const NoiseCleaner&) = default;
NoiseCleaner& NoiseCleaner::operator=(const NoiseCleaner&) = default;

bool NoiseCleaner::processFrame(
    const short* frame10MsInPtr, short* frame10MsOutPtr)
{
    return implProcessFramePcm16(frame10MsInPtr, frame10MsOutPtr);
}

bool NoiseCleaner::processFrame(
    const float* frame10MsInPtr, float* frame10MsOutPtr)
{
    return implProcessFrameFloat(frame10MsInPtr, frame10MsOutPtr);
}

size_t NoiseCleaner::getFrameSize() const
{
    return implGetFrameSize();
}

ModelId NoiseCleaner::getModelId() const
{
    return _modelId;
}

NoiseCleanerWithStats::NoiseCleanerWithStats(ModelId id) : NoiseCleaner(id)
{
}

NoiseCleanerWithStats::FrameStats NoiseCleanerWithStats::getFrameStats() const
{
    return implGetFrameStats();
}

NoiseCleanerWithStats::CumulativeStats NoiseCleanerWithStats::
    getCumulativeStats() const
{
    return implGetCumulativeStats();
}

KrispVoiceSdk::KrispVoiceSdk()
    //: _audioProcessorBuilderPtr(std::make_unique<AudioProcessorBuilder>())
    : _audioProcessorBuilderPtr(nullptr)
{
    _audioProcessorBuilderPtr = new AudioProcessorBuilder;
}

KrispVoiceSdk::~KrispVoiceSdk()
{
    delete _audioProcessorBuilderPtr;
}

void KrispVoiceSdk::registerModel(
    ModelId id, const std::wstring& path, bool preload)
{
    _audioProcessorBuilderPtr->registerModel(id, path);
    if (preload)
    {
        _audioProcessorBuilderPtr->preloadModel(id);
        _audioProcessorBuilderPtr->setModelPolicy(
            id, ModelMemoryPolicy::KeepCachedAfterLoad);
    }
    else
    {
        _audioProcessorBuilderPtr->setModelPolicy(
            id, ModelMemoryPolicy::UnloadIfNotUsed);
    }
}

void KrispVoiceSdk::registerModel(
    ModelId id, void* blobPtr, size_t blobSize, bool preload)
{
    _audioProcessorBuilderPtr->registerModel(id, blobPtr, blobSize);
    if (preload)
    {
        _audioProcessorBuilderPtr->preloadModel(id);
        _audioProcessorBuilderPtr->setModelPolicy(
            id, ModelMemoryPolicy::KeepCachedAfterLoad);
    }
    else
    {
        _audioProcessorBuilderPtr->setModelPolicy(
            id, ModelMemoryPolicy::UnloadIfNotUsed);
    }
}

constexpr ModelNameToIdMap modelMap = {
    {"model-nc-8.kw", ModelId::MicNc8K},
    {"model-nc-16.kw", ModelId::MicNc16K},
    {"model-nc-32.kw", ModelId::MicNc32K},
    {"model-bvc-32.thw", ModelId::MicBvc32K},
    {"c5.n.s.20949d.kw", ModelId::MicNc8K},
    {"c5.s.w.c9ac8f.kw", ModelId::MicNc16K},
    {"c6.f.s.ced125.kw", ModelId::MicNc32K},
    {"hs.c6.f.s.de56df.thw", ModelId::MicBvc32K}};

std::vector<ModelId> KrispVoiceSdk::registerModels(
    const std::string& modelsDirectory, bool preload)
{
    return GetModelsFromDirectory(
        *_audioProcessorBuilderPtr, modelsDirectory, modelMap);
}

void KrispVoiceSdk::unregisterModel(ModelId id)
{
    _audioProcessorBuilderPtr->unregisterModel(id);
}

std::unique_ptr<NoiseCleaner> KrispVoiceSdk::createNc(SamplingRate rate)
{
    return _audioProcessorBuilderPtr->createNc(rate);
}

std::unique_ptr<NoiseCleaner> KrispVoiceSdk::createNc(
    SamplingRate rate, ModelId modelId)
{
    return _audioProcessorBuilderPtr->createNc(rate, modelId);
}

std::unique_ptr<NoiseCleaner> KrispVoiceSdk::createBvc(
    SamplingRate rate, const std::string& device)
{
    return _audioProcessorBuilderPtr->createBvc(rate, device);
}

// std::unique_ptr<NoiseCleanerWithStats>
// KrispVoiceSdk::createNcWithStats(SamplingRate)
//{
// }
//
// std::unique_ptr<NoiseCleanerWithStats>
// KrispVoiceSdk::createNcWithStats(SamplingRate, ModelId modelId)
//{
// }
//
// std::unique_ptr<NoiseCleanerWithStats>
// KrispVoiceSdk::createBvcWithStats(SamplingRate, const std::string&)
//{
// }

void KrispVoiceSdk::loadBvcDeviceLists(
    const std::string& allowListPath, const std::string& blockListPath)
{
    _audioProcessorBuilderPtr->accessBvcDeviceManager().loadLists(
        allowListPath, blockListPath);
    ;
}

bool KrispVoiceSdk::allowBvcDevice(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().allowDevice(deviceName);
}

bool KrispVoiceSdk::blockBvcDevice(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().blockDevice(deviceName);
}

bool KrispVoiceSdk::removeBvcDevice(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().removeDevice(
        deviceName);
}

bool KrispVoiceSdk::isBvcAllowed(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().isAllowed(
        deviceName);
}

void KrispVoiceSdk::forceBvc(bool force)
{
    _audioProcessorBuilderPtr->accessBvcDeviceManager().forceBvc(force);
}

} // namespace KrispVoiceSdk
