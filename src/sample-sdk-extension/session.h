#pragma once


enum SamplingRate {
	sampling_rate_8000 = 8000,
	sampling_rate_16000 = 16000,
	sampling_rate_32000 = 32000,
	sampling_rate_44100 = 44100,
	sampling_rate_48000 = 48000
};


class Session {
public:
	virtual ~Session() = default;

	void clean_frame_pcm16(const short * frame_10ms_in_ptr, short * frame_10ms_out_ptr) {
		this->impl_clean_frame_pcm16(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}
	void clean_frame_float(const float * frame_10ms_in_ptr, float * frame_10ms_out_ptr) {
		this->impl_clean_frame_float(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}
private:
	virtual bool impl_clean_frame_pcm16(const short * f_in, short * f_out) = 0;
	virtual bool impl_clean_frame_float(const float * f_in, float * f_out) = 0;
};
