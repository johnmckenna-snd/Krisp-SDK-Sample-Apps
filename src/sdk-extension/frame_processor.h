#pragma once


namespace KrispAudioSDK
{

enum SamplingRate
{
	sampling_rate_8000 = 8000,
	sampling_rate_16000 = 16000,
	sampling_rate_32000 = 32000,
	sampling_rate_44100 = 44100,
	sampling_rate_48000 = 48000,
	sampling_rate_88200 = 88200,
	sampling_rate_96000 = 96000
};


class FrameProcessor
{
public:
	virtual ~FrameProcessor() = default;

	template <typename SamplingType>
	bool process_frame(
		const SamplingType * frame_10ms_in_ptr,
		SamplingType * frame_10ms_out_ptr)
	{
		constexpr bool is_float = std::is_same<SamplingType, float>::value;
		constexpr bool is_pcm16 = std::is_same<SamplingType, short>::value;
		static_assert(is_float || is_pcm16,
			"Only float and short audio sampling type is supported.");
	}
	template <>
	bool process_frame<short>(
		const short * frame_10ms_in_ptr, short * frame_10ms_out_ptr)
	{
		return impl_process_frame_pcm16(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}
	template <>
	bool process_frame<float>(
		const float * frame_10ms_in_ptr, float * frame_10ms_out_ptr)
	{
		return impl_process_frame_float(frame_10ms_in_ptr, frame_10ms_out_ptr);
	}

	size_t get_frame_size() const
	{
		return impl_get_frame_size();
	}

private:
	virtual size_t impl_get_frame_size() const = 0;
	virtual bool impl_process_frame_pcm16(const short * f_in, short * f_out) = 0;
	virtual bool impl_process_frame_float(const float * f_in, float * f_out) = 0;
};

}
