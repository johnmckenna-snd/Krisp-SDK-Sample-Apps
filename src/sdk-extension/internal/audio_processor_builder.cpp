#include "audio_processor_builder.h"

#include <memory>

#include <krisp-audio-sdk-nc.hpp>
#include <krisp-audio-sdk.hpp>

#include "audio_cleaner.h"
#include "krisp-exception.h"
#include "model.h"
#include "model_container_impl.h"

namespace KrispVoiceSdk
{

InternalAudioProcessorBuilder::InternalAudioProcessorBuilder() : _modelContainer(), _deviceManager(), _libraryPtr()
{
    _libraryPtr = getLibrary();
}

void InternalAudioProcessorBuilder::registerModel(ModelId id, const std::wstring& path)
{
    _modelContainer.registerModel(id, path);
}

void InternalAudioProcessorBuilder::registerModel(ModelId id, void* blobAddr, size_t blobSize)
{
    _modelContainer.registerModel(id, blobAddr, blobSize);
}

void InternalAudioProcessorBuilder::unregisterModel(ModelId id)
{
    _modelContainer.unregisterModel(id);
}

void InternalAudioProcessorBuilder::preloadModel(ModelId id)
{
    _modelContainer.preloadModel(id);
}

void InternalAudioProcessorBuilder::setModelPolicy(ModelId id, ModelMemoryPolicy policyId)
{
    if (static_cast<unsigned>(id) >= _modelContainer.getModelCount())
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    switch (policyId)
    {
        case ModelMemoryPolicy::KeepCachedAfterLoad:
            _modelContainer.enableModelOwnership(id);
            break;
        case ModelMemoryPolicy::UnloadIfNotUsed:
            _modelContainer.disableModelOwnership(id);
            break;
        default:
            throw KrispModelSelectionError("invalid policy id");
    }
}

std::shared_ptr<Model> InternalAudioProcessorBuilder::chooseModel8K()
{
    if (_modelContainer.isModelRegistered(ModelId::MicNc8K))
    {
        return getModel(ModelId::MicNc8K);
    }
    else
    {
        if (_modelContainer.isModelRegistered(ModelId::MicNc16K))
        {
            return getModel(ModelId::MicNc16K);
        }
        else
        {
            throw KrispModelSelectionError("No suitable model found for 8KHz sampling rate.");
        }
    }
}

std::shared_ptr<Model> InternalAudioProcessorBuilder::chooseModel16K()
{
    if (_modelContainer.isModelRegistered(ModelId::MicNc16K))
    {
        return getModel(ModelId::MicNc16K);
    }
    if (_modelContainer.isModelRegistered(ModelId::MicNc32K))
    {
        return getModel(ModelId::MicNc32K);
    }
    throw KrispModelSelectionError("No suitable model registered for 16KHz sampling rate.");
}

std::shared_ptr<Model> InternalAudioProcessorBuilder::chooseModel(
    const std::string& device, SamplingRate r, bool tryBvc)
{
    if (r == SamplingRate::Sr8000)
    {
        return chooseModel8K();
    }
    if (tryBvc && _deviceManager.isAllowed(device) && _modelContainer.isModelRegistered(ModelId::MicBvc32K))
    {
        return getModel(ModelId::MicBvc32K);
    }
    if (r == SamplingRate::Sr16000)
    {
        return chooseModel16K();
    }
    if (_modelContainer.isModelRegistered(ModelId::MicNc32K))
    {
        return getModel(ModelId::MicNc32K);
    }
    throw KrispModelSelectionError("No suitable model is registered for 32KHz and above sampling rates.");
}

std::unique_ptr<NoiseCleaner> InternalAudioProcessorBuilder::createOutboundNoiseCleaner(SamplingRate r)
{
    constexpr bool tryBvc = false;
    auto modelPtr = this->chooseModel("", r, tryBvc);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to choose a model");
    }
    return std::make_unique<NoiseCleanerImpl>(modelPtr, r);
}

std::unique_ptr<NoiseCleanerWithStats> InternalAudioProcessorBuilder::createOutboundNoiseCleanerWithStats(
    SamplingRate r)
{
    constexpr bool tryBvc = false;
    auto modelPtr = this->chooseModel("", r, tryBvc);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to choose a model");
    }
    return std::make_unique<NoiseCleanerWithStatsImpl>(modelPtr, r);
}

std::unique_ptr<NoiseCleaner> InternalAudioProcessorBuilder::createNoiseCleaner(SamplingRate r, ModelId model_id)
{
    auto modelPtr = this->getModel(model_id);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to load the specified model. Is it registered?");
    }
    return std::make_unique<NoiseCleanerImpl>(modelPtr, r);
}

std::unique_ptr<NoiseCleanerWithStats> InternalAudioProcessorBuilder::createNoiseCleanerWithStats(
    SamplingRate r, ModelId model_id)
{
    auto modelPtr = this->getModel(model_id);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to load the specified model. Is it registered?");
    }
    return std::make_unique<NoiseCleanerWithStatsImpl>(modelPtr, r);
}

std::unique_ptr<NoiseCleaner> InternalAudioProcessorBuilder::createOutboundBvcNoiseCleaner(
    SamplingRate r, const std::string& device)
{
    constexpr bool tryBvc = true;
    auto modelPtr = this->chooseModel(device, r, tryBvc);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to choose a model");
    }
    return std::make_unique<NoiseCleanerImpl>(modelPtr, r);
}

std::unique_ptr<NoiseCleanerWithStats> InternalAudioProcessorBuilder::createOutboundBvcNoiseCleanerWithStats(
    SamplingRate r, const std::string& device)
{
    constexpr bool tryBvc = true;
    auto modelPtr = this->chooseModel(device, r, tryBvc);
    if (!modelPtr.get())
    {
        throw KrispModelSelectionError("Failed to choose a model");
    }
    return std::make_unique<NoiseCleanerWithStatsImpl>(modelPtr, r);
}

std::shared_ptr<Model> InternalAudioProcessorBuilder::getModel(ModelId id)
{
    auto modelPtr = _modelContainer.getModel(id);
    if (!modelPtr.get())
    {
        KrispModelSelectionError("Model selection error");
    }
    return modelPtr;
}

BVCDeviceManager& InternalAudioProcessorBuilder::accessBvcDeviceManager()
{
    return this->_deviceManager;
}

} // namespace KrispVoiceSdk
