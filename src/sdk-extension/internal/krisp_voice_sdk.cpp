#include "krisp-exception.h"
#include "krisp-voice-sdk.h"

#include <memory>

#include "audio_processor_builder.h"
#include "model_scanner.h"

namespace KrispVoiceSdk
{

KrispException::KrispException(const std::string& err_msg) : m_error_msg(err_msg)
{
}

const char* KrispException::what() const noexcept
{
    return m_error_msg.c_str();
}

KrispModelLoadError::KrispModelLoadError(const std::string& err_msg) : KrispException(err_msg)
{
}

KrispModelSelectionError::KrispModelSelectionError(const std::string& err_msg) : KrispException(err_msg)
{
}

KrispDeviceListError::KrispDeviceListError(const std::string& err_msg) : KrispException(err_msg)
{
}

KrispModelScannerError::KrispModelScannerError(const std::string& err_msg) : KrispException(err_msg)
{
}

NoiseCleaner::NoiseCleaner(ModelId model_id) : _modelId(model_id)
{
}

NoiseCleaner::~NoiseCleaner()
{
}

NoiseCleaner::NoiseCleaner(const NoiseCleaner&) = default;
NoiseCleaner& NoiseCleaner::operator=(const NoiseCleaner&) = default;

bool NoiseCleaner::processFrame(const short* frame10MsInPtr, short* frame10MsOutPtr)
{
    return implProcessFramePcm16(frame10MsInPtr, frame10MsOutPtr);
}

bool NoiseCleaner::processFrame(const float* frame10MsInPtr, float* frame10MsOutPtr)
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

NoiseCleanerWithStats::CumulativeStats NoiseCleanerWithStats::getCumulativeStats() const
{
    return implGetCumulativeStats();
}

VoiceProcessorBuilder::VoiceProcessorBuilder()
    //: _audioProcessorBuilderPtr(std::make_unique<InternalAudioProcessorBuilder>())
    : _audioProcessorBuilderPtr(nullptr)
{
    _audioProcessorBuilderPtr = new InternalAudioProcessorBuilder;
}

VoiceProcessorBuilder::~VoiceProcessorBuilder()
{
    delete _audioProcessorBuilderPtr;
}

void VoiceProcessorBuilder::registerModel(ModelId id, const std::wstring& path, bool preload)
{
    _audioProcessorBuilderPtr->registerModel(id, path);
    if (preload)
    {
        _audioProcessorBuilderPtr->preloadModel(id);
        _audioProcessorBuilderPtr->setModelPolicy(id, ModelMemoryPolicy::KeepCachedAfterLoad);
    }
    else
    {
        _audioProcessorBuilderPtr->setModelPolicy(id, ModelMemoryPolicy::UnloadIfNotUsed);
    }
}

void VoiceProcessorBuilder::registerModel(ModelId id, void* blobPtr, size_t blobSize, bool preload)
{
    _audioProcessorBuilderPtr->registerModel(id, blobPtr, blobSize);
    if (preload)
    {
        _audioProcessorBuilderPtr->preloadModel(id);
        _audioProcessorBuilderPtr->setModelPolicy(id, ModelMemoryPolicy::KeepCachedAfterLoad);
    }
    else
    {
        _audioProcessorBuilderPtr->setModelPolicy(id, ModelMemoryPolicy::UnloadIfNotUsed);
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

std::vector<ModelId> VoiceProcessorBuilder::registerModels(const std::string& modelsDirectory, bool preload)
{
    return GetModelsFromDirectory(*_audioProcessorBuilderPtr, modelsDirectory, modelMap);
}

void VoiceProcessorBuilder::unregisterModel(ModelId id)
{
    _audioProcessorBuilderPtr->unregisterModel(id);
}

std::unique_ptr<NoiseCleaner> VoiceProcessorBuilder::createOutboundNoiseCleaner(SamplingRate rate)
{
    return _audioProcessorBuilderPtr->createOutboundNoiseCleaner(rate);
}

std::unique_ptr<NoiseCleanerWithStats> VoiceProcessorBuilder::createOutboundNoiseCleanerWithStats(SamplingRate rate)
{
    return _audioProcessorBuilderPtr->createOutboundNoiseCleanerWithStats(rate);
}

std::unique_ptr<NoiseCleaner> VoiceProcessorBuilder::createNoiseCleaner(SamplingRate rate, ModelId modelId)
{
    return _audioProcessorBuilderPtr->createNoiseCleaner(rate, modelId);
}

std::unique_ptr<NoiseCleaner> VoiceProcessorBuilder::createOutboundBvcNoiseCleaner(
    SamplingRate rate, const std::string& device)
{
    return _audioProcessorBuilderPtr->createOutboundBvcNoiseCleaner(rate, device);
}

std::unique_ptr<NoiseCleanerWithStats> VoiceProcessorBuilder::createOutboundBvcNoiseCleanerWithStats(
    SamplingRate rate, const std::string& device)
{
    return _audioProcessorBuilderPtr->createOutboundBvcNoiseCleanerWithStats(rate, device);
}

void VoiceProcessorBuilder::loadBvcDeviceLists(const std::string& allowListPath, const std::string& blockListPath)
{
    _audioProcessorBuilderPtr->accessBvcDeviceManager().loadLists(allowListPath, blockListPath);
    ;
}

bool VoiceProcessorBuilder::allowBvcDevice(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().allowDevice(deviceName);
}

bool VoiceProcessorBuilder::removeBvcDevice(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().removeDevice(deviceName);
}

bool VoiceProcessorBuilder::isBvcAllowed(const std::string& deviceName)
{
    return _audioProcessorBuilderPtr->accessBvcDeviceManager().isAllowed(deviceName);
}

void VoiceProcessorBuilder::forceBvc(bool force)
{
    _audioProcessorBuilderPtr->accessBvcDeviceManager().forceBvc(force);
}

} // namespace KrispVoiceSdk
