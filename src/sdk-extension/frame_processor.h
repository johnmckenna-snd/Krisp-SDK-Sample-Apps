#pragma once


namespace KrispAudioSDK
{


class FrameProcessor {
public:
	virtual ~FrameProcessor() = default;

	void process_frame_pcm16(const short * frame_10ms_in_ptr,
			short * frame_10ms_out_ptr)
	{
		this->impl_process_frame_pcm16(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}
	void process_frame_float(const float * frame_10ms_in_ptr,
			float * frame_10ms_out_ptr)
	{
		this->impl_process_frame_float(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}
private:
	virtual bool impl_process_frame_pcm16(const short * f_in, short * f_out) = 0;
	virtual bool impl_process_frame_float(const float * f_in, float * f_out) = 0;
};

}
