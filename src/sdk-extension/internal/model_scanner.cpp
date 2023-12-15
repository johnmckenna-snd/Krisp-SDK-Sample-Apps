#include <dirent.h>

#include <codecvt>
#include <locale>
#include <map>
#include <string>
#include <vector>

#include "krisp-voice-sdk.h"
#include "model_scanner.h"
#include "audio_processor_builder.h"
#include "krisp-exception.h"

namespace KrispVoiceSdk
{

class ModelCollectorFromDirectory
{
public:
    explicit ModelCollectorFromDirectory(
        const std::initializer_list<std::pair<const char*, ModelId>>&
            modelsNameToId)
        : _modelsNameToId(), _modelsIdToPath()
    {
        for (auto nameToId : modelsNameToId)
        {
            _modelsNameToId.insert(nameToId);
        }
    }

    void clear()
    {
        _modelsIdToPath.clear();
    }

    void scanDirectory(const std::string& directory)
    {
#ifdef WIN32
        constexpr char pathSeparator[] = "\\";
#else
        constexpr char pathSeparator[] = "/";
#endif
        DIR* dir = ::opendir(directory.c_str());
        if (!dir)
        {
            throw KrispModelScannerError("Can't access " + directory + ".");
        }
        struct dirent* dirItem = nullptr;
        unsigned modelsFound = 0;
        while (true)
        {
            dirItem = ::readdir(dir);
            if (!dirItem)
            {
                break;
            }
            if (dirItem->d_type != DT_REG)
            {
                continue;
            }
            auto fileName = std::string(dirItem->d_name, dirItem->d_namlen);
            auto itMicModel = _modelsNameToId.find(fileName);
            if (itMicModel != _modelsNameToId.end())
            {
                auto modelFileName = itMicModel->first;
                auto modelId = itMicModel->second;
                std::string path = directory + pathSeparator + modelFileName;
                _modelsIdToPath[modelId] = path;
                ++modelsFound;
            }
        }
        ::closedir(dir);
        if (!modelsFound)
        {
            throw KrispModelScannerError(
                "No models found in " + directory + ".");
        }
    }

    const std::map<ModelId, std::string>& getMicModelsPaths() const
    {
        return _modelsIdToPath;
    }

private:
    std::map<std::string, ModelId> _modelsNameToId;
    std::map<ModelId, std::string> _modelsIdToPath;
};

std::vector<ModelId> GetModelsFromDirectory(
    AudioProcessorBuilder& builder,
    const std::string& directory,
    const ModelNameToIdMap& modelNameToIdMap)
{
    std::vector<ModelId> foundModels;
    ModelCollectorFromDirectory modelCollector(modelNameToIdMap);
    modelCollector.scanDirectory(directory);
    foundModels.reserve(8);
    auto micModels = modelCollector.getMicModelsPaths();
    for (auto modelIdAndPath : micModels)
    {
        auto modelId = modelIdAndPath.first;
        auto modelPath = modelIdAndPath.second;
        foundModels.push_back(modelId);
        std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringConverter;
        std::wstring modelWpath = wstringConverter.from_bytes(modelPath);
        builder.registerModel(modelId, modelWpath);
    }
    return foundModels;
}

} // namespace KrispVoiceSDK
