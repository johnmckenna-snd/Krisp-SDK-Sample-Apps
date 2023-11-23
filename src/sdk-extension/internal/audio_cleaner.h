#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "audio-processor.h"
#include "model.h"


namespace KrispAudioSDK
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

	bool impl_process_frame_pcm16(const short * f_in, short * f_out) override;
	bool impl_process_frame_float(const float * f_in, float * f_out) override;
	size_t impl_get_frame_size() const override;
};


class AudioCleanerWithStats : public AudioProcessor
{
public:
	AudioCleanerWithStats(const std::shared_ptr<Model> & model_ptr, SamplingRate r);

	AudioCleanerWithStats(const AudioCleanerWithStats &);
	AudioCleanerWithStats & operator = (const AudioCleanerWithStats &);

	AudioCleanerWithStats(AudioCleanerWithStats &&);
	AudioCleanerWithStats & operator = (AudioCleanerWithStats &&);

	virtual ~AudioCleanerWithStats();

	unsigned int get_frame_voice_energy() const;
	unsigned int get_frame_noise_energy() const;

	struct TotalStats
	{
		unsigned int m_talk_time_ms;
		unsigned int m_no_noise_ms;
		unsigned int m_low_nosie_ms;
		unsigned int m_medium_noise_ms;
		unsigned int m_high_noise_ms;
	};

	Stats get_stats() const;
private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	SamplingRate m_sampling_rate;
	unsigned long m_frame_size;
	std::string m_model_alias;

	unsigned int m_frame_voice_energy;
	unsigned int m_frame_noise_energy;

	bool impl_process_frame_pcm16(const short * f_in, short * f_out) override;
	bool impl_process_frame_float(const float * f_in, float * f_out) override;
	size_t impl_get_frame_size() const override;
};

}
