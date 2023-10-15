#pragma once

#include "session_factory.h"

#include <string>
#include <memory>

#include "model.h"
#include "model_container.h"


namespace KrispAudioSDK {

class OutboundSessionFactory : public SessionFactory {
public:
	enum ModelId {
		nc_8k = 0,
		nc_16k = 1,
		nc_32k = 2,
		bvc = 3
	};
	bool register_model(const std::wstring & path, ModelId id);
	bool preload_model(ModelId id);
private:
	ModelContainer<4> m_model_container;
	std::unique_ptr<Session> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc);
};

}
