#include "audio_cleaner.h"

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>
#include <krisp-audio-sdk-nc-stats.hpp>


namespace KrispVoiceSDK
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

bool AudioCleaner::implProcessFramePcm16(const short * frame_in, short * frame_out)
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

bool AudioCleaner::implProcessFrameFloat(const float * frame_in, float * frame_out)
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

size_t AudioCleaner::implGetFrameSize() const
{
	return this->m_frame_size;
}

AudioCleanerWithStats::AudioCleanerWithStats(const std::shared_ptr<Model> & model_ptr,
		SamplingRate r) :
	AudioNoiseCleanerWithStats(model_ptr->get_id()),
	m_model_ptr(model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(r),
	m_frame_size((r * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000),
	m_model_alias()
{
	m_frameStats.m_voiceEnergy = 0;
	m_frameStats.m_noiseEnergy = 0;
	m_model_alias = model_ptr->get_given_name();
	m_krisp_session_id = krispAudioNcWithStatsCreateSession(
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
}

AudioCleanerWithStats::AudioCleanerWithStats(const AudioCleanerWithStats & copy) :
	AudioNoiseCleanerWithStats(copy.m_model_ptr->get_id()),
	m_model_ptr(copy.m_model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(copy.m_sampling_rate),
	m_frame_size(copy.m_frame_size),
	m_model_alias(copy.m_model_alias)
{
	m_frameStats.m_voiceEnergy = 0;
	m_frameStats.m_noiseEnergy = 0;
	m_krisp_session_id = krispAudioNcWithStatsCreateSession(
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
}


AudioCleanerWithStats & AudioCleanerWithStats::operator=(const AudioCleanerWithStats & copy)
{
	m_model_id = copy.m_model_id;
	m_model_ptr = copy.m_model_ptr;
	m_krisp_session_id = nullptr;
	m_sampling_rate = copy.m_sampling_rate;
	m_frame_size = copy.m_frame_size;
	m_model_alias = copy.m_model_alias;
	m_frameStats.m_voiceEnergy = 0;
	m_frameStats.m_noiseEnergy = 0;

	m_krisp_session_id = krispAudioNcWithStatsCreateSession(
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());

	return *this;
}

AudioCleanerWithStats::AudioCleanerWithStats(AudioCleanerWithStats && copy) :
	AudioNoiseCleanerWithStats(copy.m_model_ptr->get_id()),
	m_model_ptr(std::move(copy.m_model_ptr)),
	m_krisp_session_id(copy.m_krisp_session_id),
	m_sampling_rate(copy.m_sampling_rate),
	m_frame_size(copy.m_frame_size),
	m_model_alias(std::move(copy.m_model_alias))
{
	m_frameStats.m_voiceEnergy = 0;
	m_frameStats.m_noiseEnergy = 0;
	copy.m_krisp_session_id = nullptr;
}

AudioCleanerWithStats & AudioCleanerWithStats::operator = (AudioCleanerWithStats && copy)
{
	m_model_id = copy.m_model_id;
	m_model_ptr = std::move(copy.m_model_ptr);
	m_krisp_session_id = copy.m_krisp_session_id;
	copy.m_krisp_session_id = nullptr;
	m_sampling_rate = copy.m_sampling_rate;
	m_frame_size = copy.m_frame_size;
	m_model_alias = std::move(copy.m_model_alias);
	m_frameStats.m_voiceEnergy = 0;
	m_frameStats.m_noiseEnergy = 0;
	return *this;
}

bool AudioCleanerWithStats::implProcessFramePcm16(const short * frame_in, short * frame_out)
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
		m_frameStats.m_noiseEnergy = energy_info.noiseEnergy;
		m_frameStats.m_voiceEnergy = energy_info.voiceEnergy;
		return true;
	}
	m_frameStats.m_noiseEnergy = 0;
	m_frameStats.m_voiceEnergy = 0;
	// TODO: error handling
	return false;
}

bool AudioCleanerWithStats::implProcessFrameFloat(const float * frame_in, float * frame_out)
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
		m_frameStats.m_noiseEnergy = energy_info.noiseEnergy;
		m_frameStats.m_voiceEnergy = energy_info.voiceEnergy;
		return true;
	}
	m_frameStats.m_noiseEnergy = 0;
	m_frameStats.m_voiceEnergy = 0;
	// TODO: error handling
	return false;
}

AudioCleanerWithStats::CumulativeStats AudioCleanerWithStats::implGetCumulativeStats() const
{
	KrispAudioNcStats krisp_stats;
	krispAudioNcWithStatsRetrieveStats(m_krisp_session_id, &krisp_stats);
	CumulativeStats stats;
	stats.m_talkTimeMs = krisp_stats.voiceStats.talkTimeMs;
	stats.m_lowNosieMs = krisp_stats.noiseStats.lowNoiseMs;
	stats.m_mediumNoiseMs = krisp_stats.noiseStats.mediumNoiseMs;
	stats.m_highNoiseMs = krisp_stats.noiseStats.highNoiseMs;
	stats.m_noNoiseMs = krisp_stats.noiseStats.noNoiseMs;
	return stats;
}

AudioCleanerWithStats::FrameStats AudioCleanerWithStats::implGetFrameStats() const
{
	return this->m_frameStats;
}

size_t AudioCleanerWithStats::implGetFrameSize() const
{
	return this->m_frame_size;
}

}
