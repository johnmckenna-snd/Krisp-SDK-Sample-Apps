#include "model_container.h"


namespace KrispAudioSDK
{

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::register_model(unsigned long id, const std::wstring & path)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;;
	}
	m_models_data[id].m_path = path;
	return true;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::register_model(unsigned long id, void * blob_addr, size_t blob_size)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;;
	}
	m_models_data[id].m_path.clear();
	m_models_data[id].m_blob_addr = blob_addr;
	m_models_data[id].m_blob_size = blob_size;
	return true;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::unregister_model(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;;
	}
	m_models_data[id].m_path.clear();
	m_models_data[id].m_blob_addr = nullptr;
	m_models_data[id].m_blob_size = 0;
	return true;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::is_model_registered(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;
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
bool ModelContainer<ModelCount>::preload_model(unsigned long id)
{
	if (!is_model_registered(id))
	{
		m_last_error = "model is not registered";
		return false;
	}
	auto model_ptr = std::make_shared<Model>();
	// TODO: solve model name uniqueness problem
	auto given_name = std::to_string(id);
	auto path = m_models_data[id].m_path;
	bool loaded = model_ptr->load(path, given_name);
	if (loaded)
	{
		m_models_data[id].m_owning_ref = model_ptr;
		m_models_data[id].m_weak_ref = model_ptr;
		return true;
	}
	m_last_error = model_ptr->get_last_error();
	return false;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::enable_model_ownership(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;
	}
	m_models_data[id].m_keep_ownership = true;
	return true;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::disable_model_ownership(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return false;
	}
	m_models_data[id].m_keep_ownership = false;
	return true;
}

template <unsigned long ModelCount>
std::shared_ptr<Model> ModelContainer<ModelCount>::get_model(unsigned long id)
{
	if (id > ModelCount - 1)
	{
		m_last_error = "model id is out of range";
		return std::make_shared<Model>();
	}
	if (m_models_data[id].m_owning_ref.get() == nullptr)
	{
		if (m_models_data[id].m_weak_ref.expired())
		{
			if (!preload_model(id))
			{
				return std::make_shared<Model>();
			}
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

template <unsigned long ModelCount>
const std::string & ModelContainer<ModelCount>::get_last_error() const
{
	return m_last_error;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::has_error() const
{
	return m_last_error.size() ? true : false;
}

template <unsigned long ModelCount>
std::string ModelContainer<ModelCount>::pull_last_error()
{
	return std::move(m_last_error);
}

}
