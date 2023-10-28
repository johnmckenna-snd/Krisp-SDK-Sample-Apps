#pragma once

#include <string>
#include <memory>

#include "frame_processor.h"


namespace KrispAudioSDK
{

class FrameProcessorFactory
{
public:
	enum SamplingRate
	{
		sampling_rate_8000 = 8000,
		sampling_rate_16000 = 16000,
		sampling_rate_32000 = 32000,
		sampling_rate_44100 = 44100,
		sampling_rate_48000 = 48000
	};

	virtual ~FrameProcessorFactory() = default;

	std::unique_ptr<FrameProcessor> create(
		const std::string & device, SamplingRate r, bool try_bvc);
private:
	virtual std::unique_ptr<FrameProcessor> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc) = 0;
};

}
