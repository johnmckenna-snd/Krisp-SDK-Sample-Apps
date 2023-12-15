#ifndef KRISP_VOICE_SDK_H
#define KRISP_VOICE_SDK_H

#include <memory>
#include <string>
#include <vector>


namespace KrispVoiceSdk
{


enum class ModelId
{
    MicNc8K = 0,
    MicNc16K = 1,
    MicNc32K = 2,
    MicBvc32K = 3,
    SpeakerNc8K = 4,
    SpeakerNc16K = 5,
    Undefined = -1
};


enum class SamplingRate
{
    Sr8000 = 8000,
    Sr16000 = 16000,
    Sr32000 = 32000,
    Sr44100 = 44100,
    Sr48000 = 48000,
    Sr88200 = 88200,
    Sr96000 = 96000
};


class NoiseCleaner
{
public:
    explicit NoiseCleaner(ModelId model_id);
    virtual ~NoiseCleaner();

    NoiseCleaner(const NoiseCleaner&);
    NoiseCleaner& operator=(const NoiseCleaner&);

    bool processFrame(const short* frame10MsInPtr, short* frame10MsOutPtr);
    bool processFrame(const float* frame10MsInPtr, float* frame10MsOutPtr);

    size_t getFrameSize() const;
    ModelId getModelId() const;

private:
    ModelId _modelId;

    virtual size_t implGetFrameSize() const = 0;
    virtual bool implProcessFramePcm16(const short* in, short* out) = 0;
    virtual bool implProcessFrameFloat(const float* in, float* out) = 0;
};


class NoiseCleanerWithStats : public NoiseCleaner
{
public:
    explicit NoiseCleanerWithStats(ModelId id);

    struct FrameStats
    {
        unsigned _voiceEnergy = 0;
        unsigned _noiseEnergy = 0;
    };

    struct CumulativeStats
    {
        unsigned _talkTimeMs = 0;
        unsigned _noNoiseMs = 0;
        unsigned _lowNosieMs = 0;
        unsigned _mediumNoiseMs = 0;
        unsigned _highNoiseMs = 0;
    };

    FrameStats getFrameStats() const;
    CumulativeStats getCumulativeStats() const;

private:
    virtual FrameStats implGetFrameStats() const = 0;
    virtual CumulativeStats implGetCumulativeStats() const = 0;
};


class AudioProcessorBuilder;


class KrispVoiceSdk final
{
public:
    KrispVoiceSdk();
    ~KrispVoiceSdk();

    // Let the system know about the that model exists and available on the specified location. The
    // system model will be used on demand or if cached in advance.
    void registerModel(ModelId id, const std::wstring& path, bool preload);

    // Let the system know that the model is available in the memory.
    void registerModel(ModelId id, void* blobPtr, size_t blobSize, bool preload);

    /// Scan the directory for known SDK models and register all of them to the system. The function
    /// will call registerModel for each identified model in the directory.
    std::vector<ModelId> registerModels(const std::string& modelsDirectory, bool preload);

    // Let the system know that the model should not be used anymore for new noise cleaners
    void unregisterModel(ModelId id);

    // Create noise cleaner with model auto selection
    std::unique_ptr<NoiseCleaner> createNc(SamplingRate rate);

    // Create noise cleaner using the specified model
    std::unique_ptr<NoiseCleaner> createNc(SamplingRate rate, ModelId modelId);

    // Create noise cleaner with BVC if device is supported, otherwise NC model is used
    std::unique_ptr<NoiseCleaner> createBvc(SamplingRate rate, const std::string& device);

    //// Create noise cleaner with model auto selection with per frame and total noise stats support
    //std::unique_ptr<NoiseCleanerWithStats> createNcWithStats(SamplingRate);

    //// Create noise cleaner using the specified model with per frame and total noise stats support
    //std::unique_ptr<NoiseCleanerWithStats> createNcWithStats(SamplingRate, ModelId modelId);

    //// Create noise cleaner with noise stats support with BVC if device is supported, otherwise NC
    //// model is used
    //std::unique_ptr<NoiseCleanerWithStats> createBvcWithStats(SamplingRate, const std::string&);

    void loadBvcDeviceLists(const std::string& allowListPath, const std::string& blockListPath);
    bool allowBvcDevice(const std::string & deviceName);
    bool blockBvcDevice(const std::string & deviceName);
    bool removeBvcDevice(const std::string & deviceName);
    bool isBvcAllowed(const std::string & deviceName);
    void forceBvc(bool force);
private:
    AudioProcessorBuilder *_audioProcessorBuilderPtr;
};

}
#endif
