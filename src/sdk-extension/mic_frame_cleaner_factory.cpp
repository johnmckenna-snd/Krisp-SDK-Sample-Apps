#include "mic_frame_cleaner_factory.h"

#include <memory>

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>

#include "model.h"
#include "frame_cleaner.h"

#include "model_container_impl.h"


namespace KrispAudioSDK
{


//bool krispInitialized = false;
//
//
//bool InitLibrary()
//{
//	if (krispInitialized)
//	{
//		return false;
//	}
//	int result = krispAudioGlobalInit(L"");
//	if (result != 0)
//	{
//		return false;
//	}
//	krispInitialized = true;
//	return true;
//}
//
//bool UnloadLibraryResources()
//{
//	if (!krispInitialized)
//	{
//		return false;
//	}
//	int result = krispAudioGlobalDestroy();
//	if (result != 0)
//	{
//		return false;
//	}
//	krispInitialized = false;
//	return true;
//}

MicFrameCleanerFactory::MicFrameCleanerFactory() :
	m_model_container(),
	m_device_manager(),
	m_last_error(),
	m_library_ptr()
{
	m_library_ptr = get_library();
}

bool MicFrameCleanerFactory::register_model(ModelId id,
	const std::wstring & path)
{
	return m_model_container.register_model(id, path);
}

bool MicFrameCleanerFactory::preload_model(ModelId id)
{
	if (m_model_container.preload_model(id))
	{
		return true;
	}
	m_last_error = m_model_container.get_last_error();
	return false;
}

bool MicFrameCleanerFactory::set_model_policy(ModelId id,
	ModelMemoryPolicy policy_id)
{
	if (id >= m_model_container.get_model_count())
	{
		m_last_error = "model id is out of range";
		return false;
	}
	switch (policy_id)
	{
	case ModelMemoryPolicy::keep_cached_after_load:
		return m_model_container.enable_model_ownership(id);
	case ModelMemoryPolicy::unload_if_not_used:
		return m_model_container.disable_model_ownership(id);
	}
	m_last_error = "invalid policy id";
	return false;
}

bool MicFrameCleanerFactory::load_device_lists(
	const std::string & allow_list_path,
	const std::string & block_list_path)
{
	return m_device_manager.load_lists(allow_list_path, block_list_path);
}

std::shared_ptr<Model> MicFrameCleanerFactory::choose_model_8k()
{
	if (m_model_container.is_model_registered(ModelId::nc_8k))
	{
		return get_model(ModelId::nc_8k);
	}
	else
	{
		if (m_model_container.is_model_registered(ModelId::nc_16k))
		{
			return get_model(ModelId::nc_16k);
		}
		else
		{
			m_last_error = "No suitable model found for 8KHz sampling rate.";
			return std::shared_ptr<Model>();
		}
	}
}

std::shared_ptr<Model> MicFrameCleanerFactory::choose_model_16k()
{
	if (m_model_container.is_model_registered(ModelId::nc_16k))
	{
		return get_model(ModelId::nc_16k);
	}
	if (m_model_container.is_model_registered(ModelId::nc_32k))
	{
		return get_model(ModelId::nc_32k);
	}
	m_last_error = "No suitable model registered for 16KHz sampling rate.";
	return std::make_shared<Model>();
}

std::shared_ptr<Model> MicFrameCleanerFactory::choose_model(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	if (r == SamplingRate::sampling_rate_8000)
	{
		return choose_model_8k();
	}
	if (try_bvc && m_device_manager.is_allowed(device) &&
		m_model_container.is_model_registered(ModelId::bvc_32k))
	{
		return get_model(ModelId::bvc_32k);
	}
	if (r == SamplingRate::sampling_rate_16000)
	{
		return choose_model_16k();
	}
	if (m_model_container.is_model_registered(ModelId::nc_32k))
	{
		return get_model(ModelId::nc_32k);
	}
	m_last_error =
		"No suitable model is registered for 32KHz and above sampling rates.";
	return std::shared_ptr<Model>();
}

std::unique_ptr<FrameProcessor> MicFrameCleanerFactory::create_impl(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	auto model_ptr = this->choose_model(device, r, try_bvc);
	if (!model_ptr.get())
	{
		return std::unique_ptr<FrameProcessor>();
	}
	std::string model_alias = model_ptr->get_given_name();
	KrispAudioFrameDuration duration = KRISP_AUDIO_FRAME_DURATION_10MS;
	KrispAudioSessionID session = krispAudioNcCreateSession(
		static_cast<KrispAudioSamplingRate>(r),
		static_cast<KrispAudioSamplingRate>(r),
		duration,
		model_alias.c_str());
	// TODO: add error check for session
	return std::make_unique<FrameCleaner>(model_ptr, session, r, model_alias);
}

std::shared_ptr<Model> MicFrameCleanerFactory::get_model(ModelId id)
{
	auto model_ptr = m_model_container.get_model(id);
	if (!model_ptr.get())
	{
		m_last_error = m_model_container.get_last_error();
	}
	return model_ptr;
}

const std::string & MicFrameCleanerFactory::get_last_error() const
{
	return m_last_error;
}

bool MicFrameCleanerFactory::has_error() const
{
	return m_last_error.size() ? true : false;
}

}
