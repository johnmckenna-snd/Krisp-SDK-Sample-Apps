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

bool OutboundSessionFactory::load_device_lists(
	const std::string & allow_list_path,
	const std::string & block_list_path)
{
	return m_device_manager.load_lists(allow_list_path, block_list_path);
}


std::shared_ptr<Model> OutboundSessionFactory::choose_model(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	if (r == SamplingRate::sampling_rate_8000) {
		if (m_model_container.is_model_registered(ModelId::nc_8k)) {
			return m_model_container.get_model(ModelId::nc_8k);
		}
		else {
			if (m_model_container.is_model_registered(ModelId::nc_16k)) {
				return m_model_container.get_model(ModelId::nc_16k);
			}
			else {
				return std::make_shared<Model>();
			}
		}
	}

	if (try_bvc && m_device_manager.is_allowed(device)) {
		return m_model_container.get_model(ModelId::bvc);
	}

	if (r == SamplingRate::sampling_rate_16000) {
		if (m_model_container.is_model_registered(ModelId::nc_16k)) {
			m_model_container.get_model(ModelId::nc_16k);
		}
	}

	if (r == SamplingRate::sampling_rate_32000 ||
		r == SamplingRate::sampling_rate_44100 ||
		r == SamplingRate::sampling_rate_48000)
	{
		if (m_model_container.is_model_registered(ModelId::nc_32k)) {
			m_model_container.get_model(ModelId::nc_32k);
		}
	}

	return std::make_shared<Model>();
}

std::unique_ptr<Session> OutboundSessionFactory::create_impl(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	auto model_ptr = this->choose_model(device, r, try_bvc);
	if (model_ptr.get() == nullptr) {
		return std::make_unique<SessionNC>();
	}
	std::string model_alias = model_ptr->get_given_name();
	static constexpr KrispAudioFrameDuration duration =
		KRISP_AUDIO_FRAME_DURATION_10MS;
	KrispAudioSessionID session = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(r),
		static_cast<KrispAudioSamplingRate>(r),
		duration,
		model_alias.c_str());
	unsigned long frame_size = (r * duration) / 1000;
	return std::make_unique<SessionNC>(model_ptr, session, frame_size);
}

}
