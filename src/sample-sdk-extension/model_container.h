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
		if (m_models[id].get() == nullptr) {
			return false;
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

	bool is_model_registered(unsigned long id) const {
		if (m_registered_models.size() == 0) {
			return false;
		}
		if (id > m_registered_models.size() - 1) {
			return false;;
		}
		return this->m_registered_models[id];
	}

	std::shared_ptr<Model> get_model(unsigned long id) {
		if (m_registered_models.size() == 0) {
			return std::make_shared<Model>();
		}
		if (id > m_registered_models.size() - 1) {
			return std::make_shared<Model>();
		}
		auto model_ptr = m_models[id];
		if (model_ptr.get() != nullptr) {
			return model_ptr;
		}
		if (preload_model(id)) {
			auto model_ptr = m_models[id];
			if (model_ptr.get() != nullptr) {
				return model_ptr;
			}
		}
		return std::make_shared<Model>();
	}

	static constexpr unsigned long get_model_count() {
		return TModelCount;
	}

private:
	std::array<bool, TModelCount> m_registered_models = {false};
	std::array<std::wstring, TModelCount> m_models_path;
	std::array<std::shared_ptr<Model>, TModelCount> m_models;
};

}
