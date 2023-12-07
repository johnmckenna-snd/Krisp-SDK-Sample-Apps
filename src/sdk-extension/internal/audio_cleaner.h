#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "krisp-audio-processor.h"
#include "model.h"


namespace KrispVoiceSDK
{

class AudioCleaner : public AudioProcessor
{
public:
	AudioCleaner(const std::shared_ptr<Model> & model_ptr, SamplingRate r);

	AudioCleaner(const AudioCleaner &);
	AudioCleaner & operator = (const AudioCleaner &);

	AudioCleaner(AudioCleaner &&);
	AudioCleaner & operator = (AudioCleaner &&);

	virtual ~AudioCleaner();

private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	SamplingRate m_sampling_rate;
	unsigned long m_frame_size;
	std::string m_model_alias;

	bool implProcessFramePcm16(const short * f_in, short * f_out) override;
	bool implProcessFrameFloat(const float * f_in, float * f_out) override;
	size_t implGetFrameSize() const override;
};


class AudioCleanerWithStats : public AudioNoiseCleanerWithStats
{
public:
	AudioCleanerWithStats(const std::shared_ptr<Model> & model_ptr, SamplingRate r);

	AudioCleanerWithStats(const AudioCleanerWithStats &);
	AudioCleanerWithStats & operator = (const AudioCleanerWithStats &);

	AudioCleanerWithStats(AudioCleanerWithStats &&);
	AudioCleanerWithStats & operator = (AudioCleanerWithStats &&);

	virtual ~AudioCleanerWithStats() = default;
private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	SamplingRate m_sampling_rate;
	unsigned long m_frame_size;
	std::string m_model_alias;

	FrameStats m_frameStats;

	FrameStats implGetFrameStats() const override;
	CumulativeStats implGetCumulativeStats() const override;

	bool implProcessFramePcm16(const short * f_in, short * f_out) override;
	bool implProcessFrameFloat(const float * f_in, float * f_out) override;
	size_t implGetFrameSize() const override;
};

}
