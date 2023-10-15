#include "outbound_session_factory.h"

#include <memory>

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>

#include "session_nc.h"


std::unique_ptr<Session> OutboundSessionFactory::create_impl(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	// TODO: fetch the model and load it
	std::string model_alias;
	static constexpr KrispAudioFrameDuration duration =
		KRISP_AUDIO_FRAME_DURATION_10MS;
	KrispAudioSessionID session = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(r),
		static_cast<KrispAudioSamplingRate>(r),
		duration,
		model_alias.c_str());
	unsigned long frame_size = (r * duration) / 1000;
	return std::make_unique<SessionNC>(session, frame_size);
}
