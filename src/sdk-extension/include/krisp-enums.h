#pragma once


namespace KrispVoiceSDK
{

enum ModelId
{
    MicNc8K = 0,
    MicNc16K = 1,
    MicNc32K = 2,
    MicBvc32K = 3,
    SpeakerNc8K = 4,
    SpeakerNc16K = 5,
	Undefined = -1
};

enum SamplingRate
{
	Sr8000 = 8000,
	Sr16000 = 16000,
	Sr32000 = 32000,
	Sr44100 = 44100,
	Sr48000 = 48000,
	Sr88200 = 88200,
	Sr96000 = 96000
};

}
