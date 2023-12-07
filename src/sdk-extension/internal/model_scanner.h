#pragma once

#include <string>
#include <vector>

#include "krisp-audio-sdk-ext.h"
#include "audio_processor_builder.h"


namespace KrispVoiceSDK
{

std::vector<ModelId> get_models_from_directory(
    AudioProcessorBuilder &factory,
    const std::string &directory,
    const ModelNameToIdMap &name_to_id_map);

}
