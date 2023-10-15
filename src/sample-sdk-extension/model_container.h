#pragma once

#include <string>
#include <memory>

#include "model.h"


namespace KrispAudioSDK {


template <unsigned long TModelCount>
class ModelContainer {
public:
	bool register_model(const std::wstring & path, unsigned long id) {
		if (m_registered_models.size() == 0) {
			return false;
		}
		if (id > m_registered_models.size() - 1) {
			return false;;
		}
		this->m_registered_models[id] = true;
		this->m_models_path[id] = path;
		return true;
	}

	bool preload_model(unsigned long id) {
		if (m_registered_models.size() == 0) {
			return false;
		}
		if (id > m_registered_models.size() - 1) {
			return false;;
		}
		auto model_ptr = std::make_shared<Model>();
		// TODO: solve model name uniqueness problem
		auto given_name = std::to_string(id);
		auto path = this->m_models_path[id];
		bool loaded = model_ptr->load(path, given_name);
		if (loaded) {
			m_models[id] = model_ptr;
			return true;
		}
		return false;
	}

private:
	std::array<bool, TModelCount> m_registered_models = {false};
	std::array<std::wstring, TModelCount> m_models_path;
	std::array<std::shared_ptr<Model>, TModelCount> m_models;
};

}
