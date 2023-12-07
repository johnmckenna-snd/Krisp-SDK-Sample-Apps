#include "audio_processor_builder.h"

#include <memory>

#include <krisp-audio-sdk.hpp>
#include <krisp-audio-sdk-nc.hpp>

#include "krisp-exception.h"
#include "model.h"
#include "audio_cleaner.h"

#include "model_container_impl.h"


namespace KrispVoiceSDK
{


AudioProcessorBuilder::AudioProcessorBuilder() :
	m_model_container(),
	m_device_manager(),
	m_library_ptr()
{
	m_library_ptr = get_library();
}

void AudioProcessorBuilder::registerModel(ModelId id,
	const std::wstring & path)
{
	m_model_container.registerModel(id, path);
}

void AudioProcessorBuilder::registerModel(ModelId id, void * blob_addr, size_t blob_size)
{
	m_model_container.registerModel(id, blob_addr, blob_size);
}

void AudioProcessorBuilder::unregisterModel(ModelId id)
{
	m_model_container.unregisterModel(id);
}

void AudioProcessorBuilder::preloadModel(ModelId id)
{
	m_model_container.preloadModel(id);
}

void AudioProcessorBuilder::setModelPolicy(ModelId id,
	ModelMemoryPolicy policy_id)
{
	if (id >= m_model_container.get_model_count())
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	switch (policy_id)
	{
	case ModelMemoryPolicy::KeepCachedAfterLoad:
		m_model_container.enable_model_ownership(id);
		break;
	case ModelMemoryPolicy::UnloadIfNotUsed:
		m_model_container.disable_model_ownership(id);
		break;
	default:
		throw KrispModelSelectionError("invalid policy id");
	}
}

void AudioProcessorBuilder::load_device_lists(
	const std::string & allow_list_path,
	const std::string & block_list_path)
{
	m_device_manager.load_lists(allow_list_path, block_list_path);
}

std::shared_ptr<Model> AudioProcessorBuilder::choose_model_8k()
{
	if (m_model_container.is_model_registered(ModelId::MicNc8K))
	{
		return get_model(ModelId::MicNc8K);
	}
	else
	{
		if (m_model_container.is_model_registered(ModelId::MicNc16K))
		{
			return get_model(ModelId::MicNc16K);
		}
		else
		{
			throw KrispModelSelectionError("No suitable model found for 8KHz sampling rate.");
		}
	}
}

std::shared_ptr<Model> AudioProcessorBuilder::choose_model_16k()
{
	if (m_model_container.is_model_registered(ModelId::MicNc16K))
	{
		return get_model(ModelId::MicNc16K);
	}
	if (m_model_container.is_model_registered(ModelId::MicNc32K))
	{
		return get_model(ModelId::MicNc32K);
	}
	throw KrispModelSelectionError("No suitable model registered for 16KHz sampling rate.");
}

std::shared_ptr<Model> AudioProcessorBuilder::choose_model(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	if (r == SamplingRate::Sr8000)
	{
		return choose_model_8k();
	}
	if (try_bvc && m_device_manager.is_allowed(device) &&
		m_model_container.is_model_registered(ModelId::MicBvc32K))
	{
		return get_model(ModelId::MicBvc32K);
	}
	if (r == SamplingRate::Sr16000)
	{
		return choose_model_16k();
	}
	if (m_model_container.is_model_registered(ModelId::MicNc32K))
	{
		return get_model(ModelId::MicNc32K);
	}
	throw KrispModelSelectionError(
		"No suitable model is registered for 32KHz and above sampling rates.");
}

std::unique_ptr<AudioProcessor> AudioProcessorBuilder::createNc(SamplingRate r)
{
	constexpr bool try_bvc = false;
	auto model_ptr = this->choose_model("", r, try_bvc);
	if (!model_ptr.get())
	{
		throw KrispModelSelectionError("Failed to choose a model");
	}
	return std::make_unique<AudioCleaner>(model_ptr, r);
}

std::unique_ptr<AudioProcessor> AudioProcessorBuilder::createNc(SamplingRate r, ModelId model_id)
{
	auto model_ptr = this->get_model(model_id);
	if (!model_ptr.get())
	{
		throw KrispModelSelectionError("Failed to load the specified model. Is it registered?");
	}
	return std::make_unique<AudioCleaner>(model_ptr, r);
}

std::unique_ptr<AudioProcessor> AudioProcessorBuilder::createBvc(SamplingRate r,
	const std::string & device)
{
	constexpr bool try_bvc = true;
	auto model_ptr = this->choose_model(device, r, try_bvc);
	if (!model_ptr.get())
	{
		throw KrispModelSelectionError("Failed to choose a model");
	}
	return std::make_unique<AudioCleaner>(model_ptr, r);
}

std::shared_ptr<Model> AudioProcessorBuilder::get_model(ModelId id)
{
	auto model_ptr = m_model_container.get_model(id);
	if (!model_ptr.get())
	{
		KrispModelSelectionError("Model selection error");
	}
	return model_ptr;
}

}
