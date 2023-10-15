#pragma once

#include <krisp-audio-sdk.hpp>

#include "session.h"


class SessionNC : public Session {
public:
	SessionNC();
	SessionNC(KrispAudioSessionID s, unsigned long frame_size);
	~SessionNC();
private:
	KrispAudioSessionID m_krisp_session_id;
	unsigned long m_frame_size;
	virtual bool impl_clean_frame_pcm16(const short * f_in, short * f_out);
	virtual bool impl_clean_frame_float(const float * f_in, float * f_out);
};
