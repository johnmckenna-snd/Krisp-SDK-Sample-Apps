#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "model.h"

namespace KrispVoiceSdk
{

class NoiseCleanerImpl : public NoiseCleaner
{
public:
    NoiseCleanerImpl(const std::shared_ptr<Model>& model_ptr, SamplingRate r);

    NoiseCleanerImpl(const NoiseCleanerImpl&);
    NoiseCleanerImpl& operator=(const NoiseCleanerImpl&);

    NoiseCleanerImpl(NoiseCleanerImpl&&);
    NoiseCleanerImpl& operator=(NoiseCleanerImpl&&);

    virtual ~NoiseCleanerImpl();

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

class NoiseCleanerWithStatsImpl : public NoiseCleanerWithStats
{
public:
    NoiseCleanerWithStatsImpl(
        const std::shared_ptr<Model>& model_ptr, SamplingRate r);

    NoiseCleanerWithStatsImpl(const NoiseCleanerWithStatsImpl&);
    NoiseCleanerWithStatsImpl& operator=(const NoiseCleanerWithStatsImpl&);

    NoiseCleanerWithStatsImpl(NoiseCleanerWithStatsImpl&&);
    NoiseCleanerWithStatsImpl& operator=(NoiseCleanerWithStatsImpl&&);

    virtual ~NoiseCleanerWithStatsImpl() = default;

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
