#pragma once

#include "session_factory.h"

#include <string>
#include <memory>

#include "model.h"
#include "model_container.h"
#include "bvc_device_manager.h"


namespace KrispAudioSDK
{

bool InitLibrary();
bool UnloadLibraryResources();


class OutboundSessionFactory : public SessionFactory
{
public:
	enum ModelId
	{
		nc_8k = 0,
		nc_16k = 1,
		nc_32k = 2,
		bvc = 3
	};
	bool load_device_lists(const std::string & allow_list_path,
		const std::string & block_list_path);
	bool register_model(ModelId id, void * blob_ptr);
	bool register_model(ModelId id, const std::wstring & path);
	bool register_model_directory(const std::wstring & path);

	bool preload_model(ModelId id);
	const std::string & get_last_error() const;
private:
	ModelContainer<4> m_model_container;
	BVCDeviceManager m_device_manager;
	std::string m_last_error;

	std::unique_ptr<Session> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc);

	std::shared_ptr<Model> choose_model(
		const std::string & device, SamplingRate r, bool try_bvc);
};

}