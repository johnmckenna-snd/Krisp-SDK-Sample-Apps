#include "session_nc.h"

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>


namespace KrispAudioSDK {

SessionNC::SessionNC()
	: m_model_ptr(), m_krisp_session_id(nullptr), m_frame_size(0)
{
}

//SessionNC::SessionNC(const SessionNC & copy)
//	: m_model_ptr(), m_krisp_session_id(nullptr), m_frame_size(0)
//{
//}

SessionNC::SessionNC(const std::shared_ptr<Model> & model_ptr,
		KrispAudioSessionID session_id, unsigned long frame_size) :
	m_model_ptr(model_ptr),
	m_krisp_session_id(session_id),
	m_frame_size(frame_size)
{
}

SessionNC::SessionNC(SessionNC && copy)
	: m_model_ptr(), m_krisp_session_id(nullptr), m_frame_size(0)
{
	m_model_ptr = std::move(copy.m_model_ptr);
	m_krisp_session_id = copy.m_krisp_session_id;
	m_frame_size = copy.m_frame_size;
	copy.m_krisp_session_id = nullptr;
	copy.m_frame_size = 0;
}

SessionNC & SessionNC::operator = (SessionNC && copy)
{
	m_model_ptr = std::move(copy.m_model_ptr);
	m_krisp_session_id = copy.m_krisp_session_id;
	m_frame_size = copy.m_frame_size;
	copy.m_krisp_session_id = nullptr;
	copy.m_frame_size = 0;
}

SessionNC::~SessionNC()
{
	if (m_krisp_session_id) {
		int r = krispAudioNcCloseSession(m_krisp_session_id);
		if (r == 0) {
			// SUCCESS
		}
	}
}

bool SessionNC::impl_clean_frame_pcm16(const short *frame_in, short *frame_out)
{
	int r = krispAudioNcCleanAmbientNoiseInt16(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size
	);
	if (r == 0) {
		return true;
	}
	// TODO: error handling
	return false;
}

bool SessionNC::impl_clean_frame_float(const float *frame_in, float *frame_out)
{
	int r = krispAudioNcCleanAmbientNoiseFloat(
		m_krisp_session_id,
		frame_in,
		m_frame_size,
		frame_out,
		m_frame_size
	);
	if (r == 0) {
		return true;
	}
	// TODO: error handling
	return false;
}

}
