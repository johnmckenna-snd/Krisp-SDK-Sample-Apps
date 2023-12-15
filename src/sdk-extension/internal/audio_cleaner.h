#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "model.h"

namespace KrispVoiceSdk
{

class AudioCleaner : public NoiseCleaner
{
public:
    AudioCleaner(const std::shared_ptr<Model>& model_ptr, SamplingRate r);

    AudioCleaner(const AudioCleaner&);
    AudioCleaner& operator=(const AudioCleaner&);

    AudioCleaner(AudioCleaner&&);
    AudioCleaner& operator=(AudioCleaner&&);

    virtual ~AudioCleaner();

private:
    std::shared_ptr<Model> _modelPtr;
    KrispAudioSessionID _krispSessionId;
    SamplingRate _samplingRate;
    unsigned long _frameSize;
    std::string _modelAlias;

    bool implProcessFramePcm16(const short* f_in, short* f_out) override;
    bool implProcessFrameFloat(const float* f_in, float* f_out) override;
    size_t implGetFrameSize() const override;
};

class AudioCleanerWithStats : public NoiseCleanerWithStats
{
public:
    AudioCleanerWithStats(
        const std::shared_ptr<Model>& model_ptr, SamplingRate r);

    AudioCleanerWithStats(const AudioCleanerWithStats&);
    AudioCleanerWithStats& operator=(const AudioCleanerWithStats&);

    AudioCleanerWithStats(AudioCleanerWithStats&&);
    AudioCleanerWithStats& operator=(AudioCleanerWithStats&&);

    virtual ~AudioCleanerWithStats() = default;

private:
    std::shared_ptr<Model> _modelPtr;
    KrispAudioSessionID _krispSessionId;
    SamplingRate _samplingRate;
    unsigned long _frameSize;
    std::string _modelAlias;

    FrameStats _frameStats;

    FrameStats implGetFrameStats() const override;
    CumulativeStats implGetCumulativeStats() const override;

    bool implProcessFramePcm16(const short* f_in, short* f_out) override;
    bool implProcessFrameFloat(const float* f_in, float* f_out) override;
    size_t implGetFrameSize() const override;
};

} // namespace KrispVoiceSDK
