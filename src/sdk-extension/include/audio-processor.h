#pragma once

#include "krisp-audio-sdk-ext.h"


namespace KrispAudioSDK
{

class AudioProcessor
{
public:
	explicit AudioProcessor(ModelId model_id) : m_model_id(model_id)
	{
	}
	virtual ~AudioProcessor() = default;

	AudioProcessor(const AudioProcessor &) = default;
	AudioProcessor & operator = (const AudioProcessor &) = default;

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

	ModelId get_model_id() const
	{
		return m_model_id;
	}

private:
	ModelId m_model_id;
	virtual size_t impl_get_frame_size() const = 0;
	virtual bool impl_process_frame_pcm16(const short * f_in, short * f_out) = 0;
	virtual bool impl_process_frame_float(const float * f_in, float * f_out) = 0;
};

}
