#pragma once

#include <cassert>
#include <memory>
#include <string>

#include "krisp-enums.h"
#include "model.h"

namespace KrispVoiceSDK
{

template <unsigned long ModelCount>
class ModelContainer
{
public:
    void registerModel(ModelId id, const std::wstring& path);
    void registerModel(ModelId id, void* blobAddr, size_t blobSize);
    void unregisterModel(ModelId id);
    bool isModelRegistered(ModelId id);
    void preloadModel(ModelId id);
    void enableModelOwnership(ModelId id);
    void disableModelOwnership(ModelId id);
    std::shared_ptr<Model> getModel(ModelId id);
    static constexpr unsigned long getModelCount();

private:
    struct ModelData
    {
        std::wstring _path;
        void* _blobAddr = nullptr;
        size_t _blobSize = 0;
        std::shared_ptr<Model> _owningRef;
        std::weak_ptr<Model> _weakRef;
        bool _keepOwnership = true;
    };
    ModelData& accessModelData(ModelId id);
    std::array<ModelData, ModelCount> _modelsData;
};

} // namespace KrispVoiceSDK
