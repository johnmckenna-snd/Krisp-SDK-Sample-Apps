#ifndef KRISP_AUDIO_PROCESSOR_H
#define KRISP_AUDIO_PROCESSOR_H

#include <type_traits>

#include "krisp-enums.h"

namespace KrispVoiceSDK
{

class AudioProcessor
{
public:
    explicit AudioProcessor(ModelId model_id) : _modelId(model_id)
    {
    }
    virtual ~AudioProcessor() = default;

    AudioProcessor(const AudioProcessor&) = default;
    AudioProcessor& operator=(const AudioProcessor&) = default;

    template <typename SamplingType>
    bool processFrame(const SamplingType* frame_10ms_in_ptr, SamplingType* frame_10ms_out_ptr)
    {
        constexpr bool is_float = std::is_same<SamplingType, float>::value;
        constexpr bool is_pcm16 = std::is_same<SamplingType, short>::value;
        static_assert(is_float || is_pcm16, "Only float and short audio sampling type is supported.");
    }

    template <>
    bool processFrame<short>(const short* frame_10ms_in_ptr, short* frame_10ms_out_ptr)
    {
        return implProcessFramePcm16(frame_10ms_in_ptr, frame_10ms_out_ptr);
    }

    template <>
    bool processFrame<float>(const float* frame_10ms_in_ptr, float* frame_10ms_out_ptr)
    {
        return implProcessFrameFloat(frame_10ms_in_ptr, frame_10ms_out_ptr);
    }

    size_t getFrameSize() const
    {
        return implGetFrameSize();
    }

    ModelId getModelId() const
    {
        return _modelId;
    }

protected:
    ModelId _modelId;

private:
    virtual size_t implGetFrameSize() const = 0;
    virtual bool implProcessFramePcm16(const short* f_in, short* f_out) = 0;
    virtual bool implProcessFrameFloat(const float* f_in, float* f_out) = 0;
};

class AudioNoiseCleanerWithStats : public AudioProcessor
{
public:
    explicit AudioNoiseCleanerWithStats(ModelId id) : AudioProcessor(id)
    {
    }

    struct FrameStats
    {
        unsigned _voiceEnergy = 0;
        unsigned _noiseEnergy = 0;
    };

    struct CumulativeStats
    {
        unsigned _talkTimeMs = 0;
        unsigned _noNoiseMs = 0;
        unsigned _lowNosieMs = 0;
        unsigned _mediumNoiseMs = 0;
        unsigned _highNoiseMs = 0;
    };

    FrameStats getFrameStats() const
    {
        return implGetFrameStats();
    }

    CumulativeStats getCumulativeStats() const
    {
        return implGetCumulativeStats();
    }

private:
    virtual FrameStats implGetFrameStats() const = 0;
    virtual CumulativeStats implGetCumulativeStats() const = 0;
};

} // namespace KrispVoiceSDK

#endif
