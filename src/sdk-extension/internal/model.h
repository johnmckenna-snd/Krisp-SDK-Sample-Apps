#pragma once

#include <string>

#include "krisp-voice-sdk.h"
#include "library_resources.h"

namespace KrispVoiceSdk
{

class Model final
{
public:
    Model();
    ~Model();

    // the same model is supposed to be shared across multiple streams that is
    // why copy should be forbidden
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    bool load(
        const std::wstring& path, ModelId id, const std::string& givenName);
    bool load(
        void* blobAddr,
        size_t blobSize,
        ModelId id,
        const std::string& givenName);
    bool unload();
    std::string getGivenName() const;
    bool isLoaded() const;
    const std::string& getLastError() const;
    bool hasError() const;
    std::string pullLastError();
    ModelId getId() const;

private:
    ModelId _id = ModelId::Undefined;
    std::string _givenName;
    std::string _lastError;
    bool _loaded;

    std::shared_ptr<LibraryResources> _libraryPtr;
};

} // namespace KrispVoiceSDK
