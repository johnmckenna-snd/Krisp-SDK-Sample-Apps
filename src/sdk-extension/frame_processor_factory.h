#pragma once

#include <string>
#include <memory>

#include "frame_processor.h"


namespace KrispAudioSDK
{

class FrameProcessorFactory
{
public:
	virtual ~FrameProcessorFactory() = default;

	std::unique_ptr<FrameProcessor> create(
		const std::string & device, SamplingRate r, bool try_bvc);
private:
	virtual std::unique_ptr<FrameProcessor> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc) = 0;
};

}
