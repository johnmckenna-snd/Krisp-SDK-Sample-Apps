#pragma once

#include <cassert>
#include <string>
#include <memory>

#include "model.h"


namespace KrispAudioSDK
{


template <unsigned long ModelCount>
class ModelContainer
{
public:
	bool register_model(unsigned long id, const std::wstring & path);
	bool register_model(unsigned long id, void * blob_addr, size_t blob_size);
	bool unregister_model(unsigned long id);
	bool is_model_registered(unsigned long id);
	bool preload_model(unsigned long id);
	bool enable_model_ownership(unsigned long id);
	bool disable_model_ownership(unsigned long id);
	std::shared_ptr<Model> get_model(unsigned long id);
	static constexpr unsigned long get_model_count();
	const std::string & get_last_error() const;
	bool has_error() const;
	std::string pull_last_error();

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
	std::array<ModelData, ModelCount> m_models_data;
	std::string m_last_error;
};

}
