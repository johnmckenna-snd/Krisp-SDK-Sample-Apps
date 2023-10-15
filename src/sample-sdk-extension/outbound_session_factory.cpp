#include "outbound_session_factory.h"

#include <memory>

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>

#include "model.h"
#include "session_nc.h"


namespace KrispAudioSDK {


bool OutboundSessionFactory::register_model(const std::wstring & path, ModelId id)
{
	return m_model_container.register_model(path, id);
}

bool OutboundSessionFactory::preload_model(ModelId id)
{
	return m_model_container.preload_model(id);
}

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
	auto model_ptr = std::make_shared<Model>();
	return std::make_unique<SessionNC>(model_ptr, session, frame_size);
}

}
