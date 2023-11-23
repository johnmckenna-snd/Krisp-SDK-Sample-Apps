#include "audio_cleaner.h"

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>
#include <krisp-audio-sdk-nc-stats.hpp>


namespace KrispAudioSDK
{


AudioCleaner::AudioCleaner(const std::shared_ptr<Model> & model_ptr, SamplingRate r) :
	AudioProcessor(model_ptr->get_id()),
	m_model_ptr(model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(r),
	m_frame_size((r * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000),
	m_model_alias()
{
	m_model_alias = model_ptr->get_given_name();
	m_krisp_session_id = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(r),
		static_cast<KrispAudioSamplingRate>(r),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
}

AudioCleaner::~AudioCleaner()
{
	if (m_krisp_session_id)
	{
		krispAudioNcCloseSession(m_krisp_session_id);
		m_krisp_session_id = nullptr;
	}
}

AudioCleaner::AudioCleaner(const AudioCleaner & copy) :
	AudioProcessor(copy),
	m_model_ptr(copy.m_model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(copy.m_sampling_rate),
	m_frame_size(copy.m_frame_size),
	m_model_alias(copy.m_model_alias)
{
	if (m_model_ptr && m_model_ptr->is_loaded())
	{
		m_krisp_session_id = krispAudioNcCreateSession(
			static_cast<KrispAudioSamplingRate>(m_sampling_rate),
			static_cast<KrispAudioSamplingRate>(m_sampling_rate),
			KRISP_AUDIO_FRAME_DURATION_10MS,
			m_model_alias.c_str());
	}
}

bool AudioCleaner::impl_process_frame_pcm16(const short * frame_in, short * frame_out)
{
	int r = krispAudioNcCleanAmbientNoiseInt16(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size
	);
	if (r == 0)
	{
		return true;
	}
	// TODO: error handling
	return false;
}

bool AudioCleaner::impl_process_frame_float(const float * frame_in, float * frame_out)
{
	int r = krispAudioNcCleanAmbientNoiseFloat(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size
	);
	if (r == 0)
	{
		return true;
	}
	// TODO: error handling
	return false;
}

size_t AudioCleaner::impl_get_frame_size() const
{
	return this->m_frame_size;
}

AudioCleanerWithStats::AudioCleanerWithStats(const std::shared_ptr<Model> & model_ptr,
		SamplingRate r) :
	AudioProcessor(model_ptr->get_id()),
	m_model_ptr(model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(r),
	m_frame_size((r * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000),
	m_model_alias()
{
	m_model_alias = model_ptr->get_given_name();
	m_krisp_session_id = krispAudioNcWithStatsCreateSession(
		static_cast<KrispAudioSamplingRate>(r),
		static_cast<KrispAudioSamplingRate>(r),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
}

bool AudioCleanerWithStats::impl_process_frame_pcm16(const short * frame_in, short * frame_out)
{
	KrispAudioNcPerFrameInfo energy_info;
	int r = krispAudioNcWithStatsCleanAmbientNoiseInt16(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size,
		&energy_info
	);
	if (r == 0)
	{
		m_frame_noise_energy = energy_info.noiseEnergy;
		m_frame_voice_energy = energy_info.voiceEnergy;
		return true;
	}
	m_frame_noise_energy = 0;
	m_frame_voice_energy = 0;
	// TODO: error handling
	return false;
}

bool AudioCleanerWithStats::impl_process_frame_float(const float * frame_in, float * frame_out)
{
	KrispAudioNcPerFrameInfo energy_info;
	int r = krispAudioNcWithStatsCleanAmbientNoiseFloat(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size,
		&energy_info
	);
	if (r == 0)
	{
		m_frame_noise_energy = energy_info.noiseEnergy;
		m_frame_voice_energy = energy_info.voiceEnergy;
		return true;
	}
	m_frame_noise_energy = 0;
	m_frame_voice_energy = 0;
	// TODO: error handling
	return false;
}

AudioCleanerWithStats::TotalStats AudioCleanerWithStats::get_stats() const
{
	TotalStats stats;
	KrispAudioNcStats krisp_stats;
	stats.m_talk_time_ms = krisp_stats.voiceStats.talkTimeMs;
	stats.m_low_nosie_ms = krisp_stats.noiseStats.lowNoiseMs;
	stats.m_medium_noise_ms = krisp_stats.noiseStats.mediumNoiseMs;
	stats.m_high_noise_ms = krisp_stats.noiseStats.highNoiseMs;
	stats.m_no_noise_ms = krisp_stats.noiseStats.noNoiseMs;
	krispAudioNcWithStatsRetrieveStats(m_krisp_session_id, &krisp_stats);
}

unsigned int AudioCleanerWithStats::get_frame_voice_energy() const
{
	return this->m_frame_voice_energy;
}

unsigned int AudioCleanerWithStats::get_frame_noise_energy() const
{
	return this->m_frame_noise_energy;
}

size_t AudioCleanerWithStats::impl_get_frame_size() const
{
	return this->m_frame_size;
}

}
