#pragma once

#include <memory>

#include <krisp-audio-sdk.hpp>

#include "library_resources.h"
#include "frame_processor.h"
#include "model.h"


namespace KrispAudioSDK
{


class FrameCleaner : public FrameProcessor
{
public:
	FrameCleaner();
	FrameCleaner(const std::shared_ptr<Model> & model_ptr,
		KrispAudioSessionID s, SamplingRate r, const std::string & model_alias);
	FrameCleaner(const FrameCleaner & copy);
	FrameCleaner(FrameCleaner &&);
	FrameCleaner & operator = (const FrameCleaner &);
	FrameCleaner & operator = (FrameCleaner &&);
	~FrameCleaner();

private:
	std::shared_ptr<Model> m_model_ptr;
	KrispAudioSessionID m_krisp_session_id;
	SamplingRate m_sampling_rate;
	unsigned long m_frame_size;
	std::string m_model_alias;
	std::shared_ptr<LibraryResources> m_library_ptr;

	bool impl_process_frame_pcm16(const short * f_in, short * f_out) override;
	bool impl_process_frame_float(const float * f_in, float * f_out) override;
	size_t impl_get_frame_size() const override;
};

}
