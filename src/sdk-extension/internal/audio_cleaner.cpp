#include "audio_cleaner.h"

#include <krisp-audio-sdk-nc-stats.hpp>
#include <krisp-audio-sdk-nc.hpp>
#include <krisp-audio-sdk.hpp>

namespace KrispVoiceSdk
{

AudioCleaner::AudioCleaner(
    const std::shared_ptr<Model>& model_ptr, SamplingRate r)
    : NoiseCleaner(model_ptr->getId())
    , _modelPtr(model_ptr)
    , _krispSessionId(nullptr)
    , _samplingRate(r)
    , _frameSize(
          (static_cast<unsigned>(r) * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000)
    , _modelAlias()
{
    _modelAlias = model_ptr->getGivenName();
    _krispSessionId = krispAudioNcCreateSession(
        static_cast<KrispAudioSamplingRate>(r),
        static_cast<KrispAudioSamplingRate>(r),
        KRISP_AUDIO_FRAME_DURATION_10MS,
        _modelAlias.c_str());
}

AudioCleaner::~AudioCleaner()
{
    if (_krispSessionId)
    {
        krispAudioNcCloseSession(_krispSessionId);
        _krispSessionId = nullptr;
    }
}

AudioCleaner::AudioCleaner(const AudioCleaner& copy)
    : NoiseCleaner(copy)
    , _modelPtr(copy._modelPtr)
    , _krispSessionId(nullptr)
    , _samplingRate(copy._samplingRate)
    , _frameSize(copy._frameSize)
    , _modelAlias(copy._modelAlias)
{
    if (_modelPtr && _modelPtr->isLoaded())
    {
        _krispSessionId = krispAudioNcCreateSession(
            static_cast<KrispAudioSamplingRate>(_samplingRate),
            static_cast<KrispAudioSamplingRate>(_samplingRate),
            KRISP_AUDIO_FRAME_DURATION_10MS,
            _modelAlias.c_str());
    }
}

bool AudioCleaner::implProcessFramePcm16(
    const short* frame_in, short* frame_out)
{
    int r = krispAudioNcCleanAmbientNoiseInt16(
        _krispSessionId, frame_in, _frameSize, frame_out, _frameSize);
    if (r == 0)
    {
        return true;
    }
    // TODO: error handling
    return false;
}

bool AudioCleaner::implProcessFrameFloat(
    const float* frame_in, float* frame_out)
{
    int r = krispAudioNcCleanAmbientNoiseFloat(
        _krispSessionId, frame_in, _frameSize, frame_out, _frameSize);
    if (r == 0)
    {
        return true;
    }
    // TODO: error handling
    return false;
}

size_t AudioCleaner::implGetFrameSize() const
{
    return this->_frameSize;
}

AudioCleanerWithStats::AudioCleanerWithStats(
    const std::shared_ptr<Model>& model_ptr, SamplingRate r)
    : NoiseCleanerWithStats(model_ptr->getId())
    , _modelPtr(model_ptr)
    , _krispSessionId(nullptr)
    , _samplingRate(r)
    , _frameSize(
          (static_cast<unsigned>(r) * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000)
    , _modelAlias()
{
    _frameStats._voiceEnergy = 0;
    _frameStats._noiseEnergy = 0;
    _modelAlias = model_ptr->getGivenName();
    _krispSessionId = krispAudioNcWithStatsCreateSession(
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        KRISP_AUDIO_FRAME_DURATION_10MS,
        _modelAlias.c_str());
}

AudioCleanerWithStats::AudioCleanerWithStats(const AudioCleanerWithStats& copy)
    : NoiseCleanerWithStats(copy._modelPtr->getId())
    , _modelPtr(copy._modelPtr)
    , _krispSessionId(nullptr)
    , _samplingRate(copy._samplingRate)
    , _frameSize(copy._frameSize)
    , _modelAlias(copy._modelAlias)
{
    _frameStats._voiceEnergy = 0;
    _frameStats._noiseEnergy = 0;
    _krispSessionId = krispAudioNcWithStatsCreateSession(
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        KRISP_AUDIO_FRAME_DURATION_10MS,
        _modelAlias.c_str());
}

AudioCleanerWithStats& AudioCleanerWithStats::operator=(
    const AudioCleanerWithStats& copy)
{
    _modelPtr = copy._modelPtr;
    _krispSessionId = nullptr;
    _samplingRate = copy._samplingRate;
    _frameSize = copy._frameSize;
    _modelAlias = copy._modelAlias;
    _frameStats._voiceEnergy = 0;
    _frameStats._noiseEnergy = 0;

    _krispSessionId = krispAudioNcWithStatsCreateSession(
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        static_cast<KrispAudioSamplingRate>(_samplingRate),
        KRISP_AUDIO_FRAME_DURATION_10MS,
        _modelAlias.c_str());

    return *this;
}

AudioCleanerWithStats::AudioCleanerWithStats(AudioCleanerWithStats&& copy)
    : NoiseCleanerWithStats(copy._modelPtr->getId())
    , _modelPtr(std::move(copy._modelPtr))
    , _krispSessionId(copy._krispSessionId)
    , _samplingRate(copy._samplingRate)
    , _frameSize(copy._frameSize)
    , _modelAlias(std::move(copy._modelAlias))
{
    _frameStats._voiceEnergy = 0;
    _frameStats._noiseEnergy = 0;
    copy._krispSessionId = nullptr;
}

AudioCleanerWithStats& AudioCleanerWithStats::operator=(
    AudioCleanerWithStats&& copy)
{
    _modelPtr = std::move(copy._modelPtr);
    _krispSessionId = copy._krispSessionId;
    copy._krispSessionId = nullptr;
    _samplingRate = copy._samplingRate;
    _frameSize = copy._frameSize;
    _modelAlias = std::move(copy._modelAlias);
    _frameStats._voiceEnergy = 0;
    _frameStats._noiseEnergy = 0;
    return *this;
}

bool AudioCleanerWithStats::implProcessFramePcm16(
    const short* frame_in, short* frame_out)
{
    KrispAudioNcPerFrameInfo energy_info;
    int r = krispAudioNcWithStatsCleanAmbientNoiseInt16(
        _krispSessionId,
        frame_in,
        _frameSize,
        frame_out,
        _frameSize,
        &energy_info);
    if (r == 0)
    {
        _frameStats._noiseEnergy = energy_info.noiseEnergy;
        _frameStats._voiceEnergy = energy_info.voiceEnergy;
        return true;
    }
    _frameStats._noiseEnergy = 0;
    _frameStats._voiceEnergy = 0;
    // TODO: error handling
    return false;
}

bool AudioCleanerWithStats::implProcessFrameFloat(
    const float* frame_in, float* frame_out)
{
    KrispAudioNcPerFrameInfo energy_info;
    int r = krispAudioNcWithStatsCleanAmbientNoiseFloat(
        _krispSessionId,
        frame_in,
        _frameSize,
        frame_out,
        _frameSize,
        &energy_info);
    if (r == 0)
    {
        _frameStats._noiseEnergy = energy_info.noiseEnergy;
        _frameStats._voiceEnergy = energy_info.voiceEnergy;
        return true;
    }
    _frameStats._noiseEnergy = 0;
    _frameStats._voiceEnergy = 0;
    // TODO: error handling
    return false;
}

AudioCleanerWithStats::CumulativeStats AudioCleanerWithStats::
    implGetCumulativeStats() const
{
    KrispAudioNcStats krisp_stats;
    krispAudioNcWithStatsRetrieveStats(_krispSessionId, &krisp_stats);
    CumulativeStats stats;
    stats._talkTimeMs = krisp_stats.voiceStats.talkTimeMs;
    stats._lowNosieMs = krisp_stats.noiseStats.lowNoiseMs;
    stats._mediumNoiseMs = krisp_stats.noiseStats.mediumNoiseMs;
    stats._highNoiseMs = krisp_stats.noiseStats.highNoiseMs;
    stats._noNoiseMs = krisp_stats.noiseStats.noNoiseMs;
    return stats;
}

AudioCleanerWithStats::FrameStats AudioCleanerWithStats::implGetFrameStats()
    const
{
    return this->_frameStats;
}

size_t AudioCleanerWithStats::implGetFrameSize() const
{
    return this->_frameSize;
}

} // namespace KrispVoiceSDK
