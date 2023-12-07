#include "model_container.h"
#include "krisp-exception.h"


namespace KrispVoiceSDK
{

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::registerModel(unsigned long id, const std::wstring & path)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	m_models_data[id].m_path = path;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::registerModel(unsigned long id, void * blob_addr, size_t blob_size)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	m_models_data[id].m_path.clear();
	m_models_data[id].m_blob_addr = blob_addr;
	m_models_data[id].m_blob_size = blob_size;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::unregisterModel(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	m_models_data[id].m_path.clear();
	m_models_data[id].m_blob_addr = nullptr;
	m_models_data[id].m_blob_size = 0;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::is_model_registered(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	if (m_models_data[id].m_path.size())
	{
		return true;
	}
	if (m_models_data[id].m_blob_addr)
	{
		return true;
	}
	return false;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::preloadModel(unsigned long id)
{
	if (!is_model_registered(id))
	{
		throw KrispModelSelectionError("model is not registered");
	}
	auto model_ptr = std::make_shared<Model>();
	auto given_name = std::to_string(id);
	auto path = m_models_data[id].m_path;
	bool loaded = model_ptr->load(path, static_cast<ModelId>(id), given_name);
	if (!loaded)
	{
		throw KrispModelLoadError(model_ptr->get_last_error());
	}
	m_models_data[id].m_owning_ref = model_ptr;
	m_models_data[id].m_weak_ref = model_ptr;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::enable_model_ownership(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	m_models_data[id].m_keep_ownership = true;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::disable_model_ownership(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	m_models_data[id].m_keep_ownership = false;
}

template <unsigned long ModelCount>
std::shared_ptr<Model> ModelContainer<ModelCount>::get_model(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		throw KrispModelSelectionError("model id is out of range");
	}
	if (m_models_data[id].m_owning_ref.get() == nullptr)
	{
		if (m_models_data[id].m_weak_ref.expired())
		{
			preloadModel(id);
		}
		else
		{
			m_models_data[id].m_owning_ref = m_models_data[id].m_weak_ref.lock();
		}
	}
	if (m_models_data[id].m_keep_ownership)
	{
		return m_models_data[id].m_owning_ref;
	}
	else
	{
		return std::move(m_models_data[id].m_owning_ref);
	}
}

template <unsigned long ModelCount>
constexpr unsigned long ModelContainer<ModelCount>::get_model_count()
{
	return ModelCount;
}

}
