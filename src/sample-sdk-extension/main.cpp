#include <iostream>

#include "session.h"
#include "bvc_device_manager.h"
#include "outbound_session_factory.h"

#include "unit_test_device_list.h"


using KrispAudioSDK::SamplingRate;
using KrispAudioSDK::OutboundSessionFactory;


bool test() {
	OutboundSessionFactory session_factory;
	auto nc_8k_id = OutboundSessionFactory::ModelId::nc_8k;
	auto nc_16k_id = OutboundSessionFactory::ModelId::nc_16k;
	auto nc_32k_id = OutboundSessionFactory::ModelId::nc_32k;
	auto bvc_id = OutboundSessionFactory::ModelId::bvc;

	std::wstring model_nc_8k_path = L"/Users/atatalyan/dev/krisp-sdk-builds/models/standard-small/c5.n.s.20949d.kw";
	std::wstring model_nc_16k_path = L"/Users/atatalyan/dev/krisp-sdk-builds/models/standard-small/c5.s.w.c9ac8f.kw";
	std::wstring model_nc_32k_path = L"/Users/atatalyan/dev/krisp-sdk-builds/models/standard-small/c6.f.s.ced125.kw";
	std::wstring model_bvc_path = L"/Users/atatalyan/dev/krisp-sdk-builds/models/standard-small/hs.c6.f.s.de56df.thw";

	if (session_factory.register_model(model_nc_8k_path, nc_8k_id) == false) {
		assert(0);
	}
	session_factory.register_model(model_nc_16k_path, nc_16k_id);
	session_factory.register_model(model_nc_32k_path, nc_32k_id);
	session_factory.register_model(model_bvc_path, bvc_id);

	bool model_loaded = session_factory.preload_model(nc_8k_id);

	if (model_loaded) {
		std::cout << "model pre-loaded to the memory" << std::endl;
	}
	else {
		std::cerr << session_factory.get_last_error() << std::endl;
		return false;
	}

	auto session_ptr = session_factory.create("fake_id",
		SamplingRate::sampling_rate_8000, false);

	if (session_ptr.get()) {
		std::cout << "Session created" << std::endl;
	}
	else {
		std::cerr <<  "failed to create session" << std::endl;
		return false;
	}

	return true;
}


int main() {
	KrispAudioSDK::InitLibrary();
	test();
	unit_test_device_list::test_no_file_loaded();	
	unit_test_device_list::test_with_file_loaded();	
	//KrispAudioSDK::UnloadLibraryResources();
	return 0;
}
