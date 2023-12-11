#include "krisp-exception.h"
#include "model_container.h"

namespace KrispVoiceSDK
{

template <unsigned long ModelCount>
typename ModelContainer<ModelCount>::ModelData& ModelContainer<
    ModelCount>::accessModelData(ModelId id)
{
    return this->_modelsData[static_cast<unsigned long>(id)];
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::registerModel(
    ModelId id, const std::wstring& path)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    accessModelData(id)._path = path;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::registerModel(
    ModelId id, void* blobAddr, size_t blobSize)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    accessModelData(id)._path.clear();
    accessModelData(id)._blobAddr = blobAddr;
    accessModelData(id)._blobSize = blobSize;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::unregisterModel(ModelId id)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    accessModelData(id)._path.clear();
    accessModelData(id)._blobAddr = nullptr;
    accessModelData(id)._blobSize = 0;
}

template <unsigned long ModelCount>
bool ModelContainer<ModelCount>::isModelRegistered(ModelId id)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    if (accessModelData(id)._path.size())
    {
        return true;
    }
    if (accessModelData(id)._blobAddr)
    {
        return true;
    }
    return false;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::preloadModel(ModelId id)
{
    if (!isModelRegistered(id))
    {
        throw KrispModelSelectionError("model is not registered");
    }
    auto model_ptr = std::make_shared<Model>();
    auto given_name = std::to_string(static_cast<unsigned>(id));
    auto path = accessModelData(id)._path;
    bool loaded = model_ptr->load(path, id, given_name);
    if (!loaded)
    {
        throw KrispModelLoadError(model_ptr->getLastError());
    }
    accessModelData(id)._owningRef = model_ptr;
    accessModelData(id)._weakRef = model_ptr;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::enableModelOwnership(ModelId id)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    accessModelData(id)._keepOwnership = true;
}

template <unsigned long ModelCount>
void ModelContainer<ModelCount>::disableModelOwnership(ModelId id)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    accessModelData(id)._keepOwnership = false;
}

template <unsigned long ModelCount>
std::shared_ptr<Model> ModelContainer<ModelCount>::getModel(ModelId id)
{
    if (static_cast<unsigned>(id) > ModelCount - 1)
    {
        throw KrispModelSelectionError("model id is out of range");
    }
    if (accessModelData(id)._owningRef.get() == nullptr)
    {
        if (accessModelData(id)._weakRef.expired())
        {
            preloadModel(id);
        }
        else
        {
            accessModelData(id)._owningRef =
                accessModelData(id)._weakRef.lock();
        }
    }
    if (accessModelData(id)._keepOwnership)
    {
        return accessModelData(id)._owningRef;
    }
    else
    {
        return std::move(accessModelData(id)._owningRef);
    }
}

template <unsigned long ModelCount>
constexpr unsigned long ModelContainer<ModelCount>::getModelCount()
{
    return ModelCount;
}

} // namespace KrispVoiceSDK
