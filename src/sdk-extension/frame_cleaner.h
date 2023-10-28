#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "frame_processor.h"
#include "model.h"


namespace KrispAudioSDK
{


class FrameCleaner : public FrameProcessor
{
public:
	FrameCleaner();
	FrameCleaner(const std::shared_ptr<Model> & model_ptr,
		KrispAudioSessionID s, unsigned long frame_size);

	// TODO: add function to create a new session handle on copy
	FrameCleaner(const FrameCleaner & copy) = delete;
	FrameCleaner(FrameCleaner &&);

	FrameCleaner & operator = (const FrameCleaner &) = delete;
	FrameCleaner & operator = (FrameCleaner &&);

	~FrameCleaner();
private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	unsigned long m_frame_size;
	virtual bool impl_process_frame_pcm16(const short * f_in, short * f_out)
		override;
	virtual bool impl_process_frame_float(const float * f_in, float * f_out)
		override;
};

}
