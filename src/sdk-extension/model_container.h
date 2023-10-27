#pragma once

#include <cassert>
#include <string>
#include <memory>

#include "model.h"


namespace KrispAudioSDK
{


template <unsigned long TModelCount>
class ModelContainer
{
public:
	bool register_model(unsigned long id, const std::wstring & path)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;;
		}
		m_models_data[id].m_path = path;
		return true;
	}

	bool register_model(unsigned long id, void * blob_addr, size_t blob_size)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;;
		}
		m_models_data[id].m_path.clear();
		m_models_data[id].m_blob_addr = blob_addr;
		m_models_data[id].m_blob_size = blob_size;
		return true;
	}

	bool is_model_registered(unsigned long id)
	{
		if (id > TModelCount - 1)
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

	bool preload_model(unsigned long id)
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

	bool enable_model_ownership(unsigned long id)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;
		}
		m_models_data[id].m_keep_ownership = true;
		return true;
	}

	bool disable_model_ownership(unsigned long id)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;
		}
		m_models_data[id].m_keep_ownership = false;
		return true;
	}

	std::shared_ptr<Model> get_model(unsigned long id)
	{
		if (id > TModelCount - 1)
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

	static constexpr unsigned long get_model_count()
	{
		return TModelCount;
	}

	const std::string & get_last_error() const
	{
		return m_last_error;
	}

	bool has_error() const
	{
		return m_last_error.size() ? true : false;
	}

	std::string pull_last_error()
	{
		return std::move(m_last_error);
	}

private:
	struct ModelData
	{
		std::wstring m_path;
		void * m_blob_addr = nullptr;
		size_t m_blob_size = 0;
		std::shared_ptr<Model> m_owning_ref;
		std::weak_ptr<Model> m_weak_ref;
		bool m_keep_ownership = true;
	};
	std::array<ModelData, TModelCount> m_models_data;
	std::string m_last_error;
};

}
