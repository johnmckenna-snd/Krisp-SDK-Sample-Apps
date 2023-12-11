#pragma once

#include <string>
#include <vector>

#include "audio_processor_builder.h"
#include "krisp-voice-sdk.h"

namespace KrispVoiceSDK
{

std::vector<ModelId> GetModelsFromDirectory(
    AudioProcessorBuilder& factory,
    const std::string& directory,
    const ModelNameToIdMap& nameToIdMap);

}
