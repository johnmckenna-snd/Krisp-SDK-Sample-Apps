#pragma once

#include <string>
#include <vector>

#include "mic_frame_cleaner_factory.h"


namespace KrispAudioSDK
{

bool load_models_from_directory(
	MicFrameCleanerFactory & factory,
	const std::string & directory
);

}
