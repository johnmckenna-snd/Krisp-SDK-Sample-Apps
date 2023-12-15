#include "model.h"

#include <iostream>
#include <krisp-audio-sdk.hpp>

namespace KrispVoiceSdk
{

Model::Model() : _givenName(), _lastError(), _loaded(false), _libraryPtr()
{
}

Model::~Model()
{
    unload();
}

bool Model::load(
    const std::wstring& path, ModelId modelId, const std::string& givenName)
{
    if (_loaded)
    {
        _lastError = "Please use Model::unload() first to unload the model.";
        return false;
    }
    if (!_libraryPtr.get())
    {
        _libraryPtr = getLibrary();
    }
    int result = krispAudioSetModel(path.c_str(), givenName.c_str());
    if (result != 0)
    {
        _lastError = "Failed to load the model";
        _libraryPtr.reset();
        return false;
    }
    _loaded = true;
    _id = modelId;
    _givenName = givenName;
    return true;
}

bool Model::load(
    void* blobPtr,
    size_t blobSize,
    ModelId modelId,
    const std::string& givenName)
{
    if (_loaded)
    {
        _lastError = "Please use Model::unload() first to unload the model.";
        return false;
    }
    if (!_libraryPtr)
    {
        _libraryPtr = getLibrary();
    }
    int result = krispAudioSetModelBlob(blobPtr, blobSize, givenName.c_str());
    if (result != 0)
    {
        _lastError = "Failed to load the model";
        _libraryPtr.reset();
        return false;
    }
    _loaded = true;
    _id = modelId;
    _givenName = givenName;
    return true;
}

bool Model::unload()
{
    if (_loaded)
    {
        int result = krispAudioRemoveModel(_givenName.c_str());
        if (result != 0)
        {
            return false;
        }
        _givenName.clear();
        _loaded = false;
        _libraryPtr.reset();
        return true;
    }
    return false;
}

ModelId Model::getId() const
{
    return _id;
}

std::string Model::getGivenName() const
{
    return _givenName;
}

bool Model::isLoaded() const
{
    return _loaded;
}

const std::string& Model::getLastError() const
{
    return _lastError;
}

bool Model::hasError() const
{
    return _lastError.size() ? true : false;
}

std::string Model::pullLastError()
{
    return std::move(_lastError);
}

} // namespace KrispVoiceSDK
