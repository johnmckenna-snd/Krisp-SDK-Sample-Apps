#include "library_resources.h"

#include <krisp-audio-sdk.hpp>


namespace KrispAudioSDK
{

LibraryResources::LibraryResources()
{
	int result = krispAudioGlobalInit(L"");
	if (result != 0)
	{
		// error
	}
}

LibraryResources::~LibraryResources()
{
	int result = krispAudioGlobalDestroy();
	if (result != 0)
	{
		// error
	}
}

std::shared_ptr<LibraryResources> get_library()
{
	static std::weak_ptr<LibraryResources> weak_ptr;
	if (weak_ptr.expired())
	{
		auto shared_ptr = std::make_shared<LibraryResources>();
		weak_ptr = shared_ptr;
		return shared_ptr;
	}
	return weak_ptr.lock();
}

}
