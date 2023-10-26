#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "session.h"
#include "model.h"


namespace KrispAudioSDK {


class SessionNC : public Session {
public:
	SessionNC();
	SessionNC(const std::shared_ptr<Model> & model_ptr,
		KrispAudioSessionID s, unsigned long frame_size);

	// TODO: add function to create a new session handle on copy
	SessionNC(const SessionNC & copy) = delete;
	SessionNC(SessionNC &&);

	SessionNC & operator = (const SessionNC &) = delete;
	SessionNC & operator = (SessionNC &&);

	~SessionNC();
private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	unsigned long m_frame_size;
	virtual bool impl_clean_frame_pcm16(const short * f_in, short * f_out);
	virtual bool impl_clean_frame_float(const float * f_in, float * f_out);
};

}
