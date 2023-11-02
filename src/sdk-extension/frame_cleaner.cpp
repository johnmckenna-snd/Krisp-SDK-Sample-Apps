#include "frame_cleaner.h"

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>


namespace KrispAudioSDK
{

FrameCleaner::FrameCleaner()
	: m_model_ptr(), m_krisp_session_id(nullptr), m_frame_size(0),
	m_library_ptr()
{
	m_library_ptr = get_library();
}

//FrameCleaner::FrameCleaner(const FrameCleaner & copy)
//	: m_model_ptr(), m_krisp_session_id(nullptr), m_frame_size(0)
//{
//}

FrameCleaner::FrameCleaner(const std::shared_ptr<Model> & model_ptr,
		KrispAudioSessionID session_id,
		SamplingRate r,
		const std::string & model_alias) :
	m_model_ptr(model_ptr),
	m_krisp_session_id(session_id),
	m_sampling_rate(r),
	m_frame_size((r * KRISP_AUDIO_FRAME_DURATION_10MS) / 1000),
	m_model_alias(model_alias),
	m_library_ptr()
{
	m_library_ptr = get_library();
}

FrameCleaner::FrameCleaner(const FrameCleaner & copy) :
	m_model_ptr(copy.m_model_ptr),
	m_krisp_session_id(nullptr),
	m_sampling_rate(copy.m_sampling_rate),
	m_frame_size(copy.m_frame_size),
	m_model_alias(copy.m_model_alias),
	m_library_ptr(copy.m_library_ptr)
{
	m_krisp_session_id = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
	// TODO: add error check
}

FrameCleaner::FrameCleaner(FrameCleaner && copy) :
	m_model_ptr(std::move(copy.m_model_ptr)),
	m_krisp_session_id(copy.m_krisp_session_id),
	m_sampling_rate(copy.m_sampling_rate),
	m_frame_size(copy.m_frame_size),
	m_model_alias(std::move(copy.m_model_alias)),
	m_library_ptr(std::move(copy.m_library_ptr))
{
	copy.m_krisp_session_id = nullptr;
}

FrameCleaner & FrameCleaner::operator = (const FrameCleaner & copy)
{
	if (m_krisp_session_id) {
		int r = krispAudioNcCloseSession(m_krisp_session_id);
		if (r == 0) {
			// SUCCESS
		}
	}
	m_model_ptr = copy.m_model_ptr;
	m_sampling_rate = copy.m_sampling_rate;
	m_frame_size = copy.m_frame_size;
	m_model_alias = copy.m_model_alias;
	m_krisp_session_id = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		static_cast<KrispAudioSamplingRate>(m_sampling_rate),
		KRISP_AUDIO_FRAME_DURATION_10MS,
		m_model_alias.c_str());
	// TODO: add error check
	return *this;
}

FrameCleaner & FrameCleaner::operator = (FrameCleaner && copy)
{
	if (m_krisp_session_id) {
		int r = krispAudioNcCloseSession(m_krisp_session_id);
		if (r == 0) {
			// SUCCESS
		}
	}
	m_model_ptr = std::move(copy.m_model_ptr);
	m_krisp_session_id = copy.m_krisp_session_id;
	m_sampling_rate = copy.m_sampling_rate;
	m_frame_size = copy.m_frame_size;
	m_model_alias = std::move(copy.m_model_alias);
	copy.m_krisp_session_id = nullptr;
	return *this;
}

FrameCleaner::~FrameCleaner()
{
	if (m_krisp_session_id) {
		int r = krispAudioNcCloseSession(m_krisp_session_id);
		if (r == 0) {
			// SUCCESS
		}
	}
}

bool FrameCleaner::impl_process_frame_pcm16(const short *frame_in,
	short *frame_out)
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

bool FrameCleaner::impl_process_frame_float(const float *frame_in,
	float *frame_out)
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

size_t FrameCleaner::impl_get_frame_size() const
{
	constexpr size_t frame_duration = 10;
	return (m_sampling_rate * frame_duration) / 1000;
}

}
