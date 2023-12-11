#include "library_resources.h"

#include <krisp-audio-sdk.hpp>

namespace KrispVoiceSDK
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

std::shared_ptr<LibraryResources> getLibrary()
{
    static std::weak_ptr<LibraryResources> weak_ptr;
    if (weak_ptr.expired())
    {
        auto sharedPtr = std::make_shared<LibraryResources>();
        weak_ptr = sharedPtr;
        return sharedPtr;
    }
    return weak_ptr.lock();
}

} // namespace KrispVoiceSDK
