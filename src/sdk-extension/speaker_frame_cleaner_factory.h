#include "speaker_frame_cleaner_factory.h"


class SpeakerFrameCleanerFactory : public FrameProcessorFactory
{
public:
	enum ModelId
	{
		speaker_nc_8k = 0,
		speaker_nc_16k = 1
	};

};
