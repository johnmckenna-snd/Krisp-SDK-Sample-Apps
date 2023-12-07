#pragma once

#include <type_traits>

#include "krisp-enums.h"


namespace KrispVoiceSDK
{

class AudioProcessor
{
public:
	explicit AudioProcessor(ModelId model_id) : m_model_id(model_id)
	{
	}
	virtual ~AudioProcessor() = default;

	AudioProcessor(const AudioProcessor &) = default;
	AudioProcessor & operator = (const AudioProcessor &) = default;

	template <typename SamplingType>
	bool processFrame(
		const SamplingType * frame_10ms_in_ptr,
		SamplingType * frame_10ms_out_ptr)
	{
		constexpr bool is_float = std::is_same<SamplingType, float>::value;
		constexpr bool is_pcm16 = std::is_same<SamplingType, short>::value;
		static_assert(is_float || is_pcm16,
			"Only float and short audio sampling type is supported.");
	}

	template <>
	bool processFrame<short>(
		const short * frame_10ms_in_ptr, short * frame_10ms_out_ptr)
	{
		return implProcessFramePcm16(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}

	template <>
	bool processFrame<float>(
		const float * frame_10ms_in_ptr, float * frame_10ms_out_ptr)
	{
		return implProcessFrameFloat(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}

	size_t getFrameSize() const
	{
		return implGetFrameSize();
	}

	ModelId getModelId() const
	{
		return m_model_id;
	}

protected:
	ModelId m_model_id;
private:
	virtual size_t implGetFrameSize() const = 0;
	virtual bool implProcessFramePcm16(const short * f_in, short * f_out) = 0;
	virtual bool implProcessFrameFloat(const float * f_in, float * f_out) = 0;
};

class AudioNoiseCleanerWithStats : public AudioProcessor
{
public:
	explicit AudioNoiseCleanerWithStats(ModelId id) : AudioProcessor(id)
	{
	}

	struct FrameStats
	{
		unsigned m_voiceEnergy = 0;
		unsigned m_noiseEnergy = 0;
	};

	struct CumulativeStats
	{
		unsigned m_talkTimeMs = 0;
		unsigned m_noNoiseMs = 0;
		unsigned m_lowNosieMs = 0;
		unsigned m_mediumNoiseMs = 0;
		unsigned m_highNoiseMs = 0;
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


}
