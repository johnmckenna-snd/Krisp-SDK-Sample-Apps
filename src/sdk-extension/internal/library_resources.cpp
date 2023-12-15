#include "library_resources.h"

#include <krisp-audio-sdk.hpp>

namespace KrispVoiceSdk
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
    static std::weak_ptr<LibraryResources> weakPtr;
    if (weakPtr.expired())
    {
        auto sharedPtr = std::make_shared<LibraryResources>();
        weakPtr = sharedPtr;
        return sharedPtr;
    }
    return weakPtr.lock();
}

} // namespace KrispVoiceSDK
