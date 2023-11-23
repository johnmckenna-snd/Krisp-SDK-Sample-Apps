#pragma once

#include <string>

#include "krisp-audio-sdk-ext.h"
#include "library_resources.h"


namespace KrispAudioSDK
{

class Model final
{
public:
	Model();
	~Model();

	// the same model is supposed to be shared across multiple streams that is why copy
	// should be forbidden
	Model(const Model &) = delete;
	Model & operator = (const Model &) = delete;

	Model(Model &&) = default;
	Model & operator = (Model &&) = default;

	bool load(const std::wstring &path, ModelId model_id, const std::string &given_name);
	bool load(void *blob_ptr, size_t blob_size, ModelId model_id, const std::string &given_name);
	bool unload();
	std::string get_given_name() const;
	bool is_loaded() const;
	const std::string &get_last_error() const;
	bool has_error() const;
	std::string pull_last_error();
	ModelId get_id() const;

private:
	ModelId m_id = ModelId::undefined;
	std::string m_given_name;
	std::string m_last_error;
	bool m_loaded;

	std::shared_ptr<LibraryResources> m_library_ptr;
};

}
