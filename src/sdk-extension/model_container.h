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
	bool register_model(const std::wstring & path, unsigned long id)
	{
		//static_assert(TModelCount != 3, "LALALALALA");
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;;
		}
		this->m_models_path[id] = path;
		return true;
	}

	bool is_model_registered(unsigned long id)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return false;
		}
		return this->m_models_path[id].size() ? true : false;
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
		auto path = this->m_models_path[id];
		bool loaded = model_ptr->load(path, given_name);
		if (loaded)
		{
			m_models[id] = model_ptr;
			m_models_refs[id] = model_ptr;
			return true;
		}
		m_last_error = model_ptr->get_last_error();
		return false;
	}

	std::shared_ptr<Model> get_model(unsigned long id)
	{
		if (id > TModelCount - 1)
		{
			m_last_error = "model id is out of range";
			return std::make_shared<Model>();
		}
		if (m_models[id].get() == nullptr)
		{
			if (m_models_refs[id].expired())
			{
				if (!preload_model(id))
				{
					return std::make_shared<Model>();
				}
			}
			else
			{
				m_models[id] = m_models_refs[id].lock();
			}
		}
		return std::move(m_models[id]);
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
	std::array<std::wstring, TModelCount> m_models_path;
	std::array<std::shared_ptr<Model>, TModelCount> m_models;
	std::array<std::weak_ptr<Model>, TModelCount> m_models_refs;
	std::string m_last_error;
};

}
