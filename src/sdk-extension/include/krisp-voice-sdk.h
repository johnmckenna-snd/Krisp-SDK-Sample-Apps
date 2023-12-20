#ifndef KRISP_VOICE_SDK_H
#define KRISP_VOICE_SDK_H

#include <memory>
#include <string>
#include <vector>

namespace KrispVoiceSdk
{

/// @brief the identifier for each compatible model
enum class ModelId
{
    /// @brief the id of the uplink NC 8KHz model
    MicNc8K = 0,

    /// @brief the id of the uplink NC 16KHz model
    MicNc16K = 1,

    /// @brief the id of the uplink NC 32KHz model
    MicNc32K = 2,

    /// @brief the id of the uplink BVC 32KHz model
    MicBvc32K = 3,

    /// @brief the id of the downlink NC 8KHz model
    SpeakerNc8K = 4,

    /// @brief the id of the downlink NC 16KHz model
    SpeakerNc16K = 5,

    /// @brief default value for unspecified model
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

/// @brief the base class for voice each voice processing module.
/// The interface provides methods to process PCM16 and FLOAT32 audio frames.
/// It provides the link to the AI model used for voice processing and provides fixed frame size in
/// number of audio samples. Each audio stream should be used with separate voice processing module.
/// @attention you should not use the same voice processing module for two different audio streams.
/// @throws KrispException on methods
class NoiseCleaner
{
public:
    /// @brief you own the object and you are responsible to destroy the object
    virtual ~NoiseCleaner();

    /// @brief process PCM16 audio frame, the input frame will not be modified.
    /// @param[in] frame10MsInPtr the pointer to the input frame
    /// @param[out] frame10MsOutPtr the pointer to the buffer where modified frame will be written
    /// @return true on success
    bool processFrame(const short* frame10MsInPtr, short* frame10MsOutPtr);

    /// @brief process FLOAT32 audio frame, the input frame will not be modified.
    /// @param[in] frame10MsInPtr the pointer to the input frame
    /// @param[out] frame10MsOutPtr the pointer to the buffer where modified frame will be written
    /// @return true on success
    bool processFrame(const float* frame10MsInPtr, float* frame10MsOutPtr);

    /// @brief get the size of the audio frame, both input and output frame have the same size.
    /// @return the size of the frame in number of samples
    size_t getFrameSize() const;

    /// @brief  get the identifier of the model used for the audio processing
    /// @return the identifier of the model
    ModelId getModelId() const;

protected:
    /// @brief the object is built internally, you are not supposed to build it
    /// @param[in] model_id the identifier of the AI model
    explicit NoiseCleaner(ModelId model_id);
    NoiseCleaner(const NoiseCleaner&);
    NoiseCleaner& operator=(const NoiseCleaner&);

private:
    ModelId _modelId;

    virtual size_t implGetFrameSize() const = 0;
    virtual bool implProcessFramePcm16(const short* in, short* out) = 0;
    virtual bool implProcessFrameFloat(const float* in, float* out) = 0;
};

class NoiseCleanerWithStats : public NoiseCleaner
{
public:
    struct FrameStats
    {
        /// @brief the voice energy from 0 to 100
        unsigned _voiceEnergy = 0;
        /// @brief the noise energy from 0 to 100
        unsigned _noiseEnergy = 0;
    };
    FrameStats getFrameStats() const;

    struct CumulativeStats
    {
        /// @brief total talk time in milliseconds during the session
        unsigned _talkTimeMs = 0;
        /// @brief total no noise time in milliseconds
        unsigned _noNoiseMs = 0;
        /// @brief total low noise time in milliseconds
        unsigned _lowNosieMs = 0;
        /// @brief total medium noise time in milliseconds
        unsigned _mediumNoiseMs = 0;
        /// @brief total high noise time in milliseconds
        /// @warning the medium, high and low noise time exclude each other
        unsigned _highNoiseMs = 0;
    };
    CumulativeStats getCumulativeStats() const;

protected:
    /// @brief the object is built internally, you are not supposed to build it
    /// @param[in] model_id the identifier of the AI model
    explicit NoiseCleanerWithStats(ModelId id);

private:
    virtual FrameStats implGetFrameStats() const = 0;
    virtual CumulativeStats implGetCumulativeStats() const = 0;
};

/// Forward reference to internal structure.
class InternalAudioProcessorBuilder;

/// @brief Please maintain a single instance of the class for building voice processing modules,
/// including noise cleaners, noise cleaners with stats support, voice accent processors, ASR
/// processing modules.
class VoiceProcessorBuilder final
{
public:
    VoiceProcessorBuilder();
    ~VoiceProcessorBuilder();

    /// @brief Let the system know the availability of the AI model on the file system.
    /// @param[in] id the unique identifier of the model.
    /// @param[in] path the path to the weight file
    /// @param[in] preload true if the model should be kept cached in the memory, false if should be
    /// kept in the memory on demand only and released if not used
    void registerModel(ModelId id, const std::wstring& path, bool preload);

    /// @brief Let the system know the availability of the AI model in the memory
    /// @param[in] id the unique identifier of the model.
    /// @param[in] blobPtr the location of the model in the memory
    /// @param[in] blobSize the size of the model in the memory
    /// @param[in] preload true if the model should be kept cached in the memory, false if should be
    /// kept in the memory on demand only and released if not used
    /// @attention make sure to keep the content available at the blobPtr address if preload is
    /// set to false, please release blobPtr content if preload is set to true.
    void registerModel(ModelId id, void* blobPtr, size_t blobSize, bool preload);

    /// @brief Scan the directory for known AI models, the registerModel will be invoked for each
    /// identified model in the directory.
    /// @param[in] modelsDirectory the path to the directory where known models are stored
    /// @param[in] preload true if identified models should be kept cached in the memory, false if
    /// should be kept in the memory on demand only and released if not used
    /// @return
    std::vector<ModelId> registerModels(const std::string& modelsDirectory, bool preload);

    // Let the system know that the model should not be used anymore for new noise cleaners

    /// @brief Make sure that new voice processors will not use the specified model.
    /// @param[in] id the unique identifier of the model.
    void unregisterModel(ModelId id);

    /// @brief Create noise cleaner module with automatic outbound model selection, the optimum
    /// model is chosen within registered outbound models considering the sampling rate.
    /// @param[in] rate the sampling rate of the audio stream
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleaner> createOutboundNoiseCleaner(SamplingRate rate);

    /// @brief Create noise cleaner for outbound or inbound audio using explicitly specified model.
    /// @param[in] rate the sampling rate of the audio stream
    /// @param[in] modelId the identifier of the registered model
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleaner> createNoiseCleaner(SamplingRate rate, ModelId modelId);

    /// @brief Create noise cleaner modul with automatic model selection, the best model is chosen
    /// within registered outbound models considering the sampling rate of the device and the
    /// BVC support of the device, the BVC models will be chosen if the device and sampling rate
    /// requirements are met, otherwise regular noise canceling models will be used.
    /// @param[in] rate sampling rate of the audio
    /// @param[in] device the name of the audio device
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleaner> createOutboundBvcNoiseCleaner(SamplingRate rate, const std::string& device);

    /// @brief Create noise cleaner module with automatic outbound model selection, the optimum
    /// model is chosen within registered outbound models considering the sampling rate.
    /// @param[in] rate the sampling rate of the audio stream
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleanerWithStats> createOutboundNoiseCleanerWithStats(SamplingRate);

    /// @brief Create noise cleaner for outbound or inbound audio using explicitly specified model.
    /// @param[in] rate the sampling rate of the audio stream
    /// @param[in] modelId the identifier of the registered model
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleanerWithStats> createNoiseCleanerWithStats(SamplingRate rate, ModelId modelId);

    /// @brief Create noise cleaner modul with automatic model selection, the best model is chosen
    /// within registered outbound models considering the sampling rate of the device and the
    /// BVC support of the device, the BVC models will be chosen if the device and sampling rate
    /// requirements are met, otherwise regular noise canceling models will be used.
    /// @param[in] rate sampling rate of the audio
    /// @param[in] device the name of the audio device
    /// @return the NoiseCleaner object with the ownership
    std::unique_ptr<NoiseCleanerWithStats> createOutboundBvcNoiseCleanerWithStats(
        SamplingRate rate, const std::string& device);

    /// @brief Specify the allow list and block list for BVC devices.
    /// @param[in] allowListPath the path to the BVC allow list
    /// @param[in] blockListPath the path to the BVC block list
    void loadBvcDeviceLists(const std::string& allowListPath, const std::string& blockListPath);

    /// @brief Allow the device with Krisp BVC tech, the allow list file will be updated.
    /// @param[in] deviceName the name of the device
    /// @return true on success
    bool allowBvcDevice(const std::string& deviceName);

    ///// @brief Do not allow the device for Krisp BVC tech, the block list file will be updated.
    ///// @param[in] deviceName the name of the device
    ///// @return true on success
    //bool blockBvcDevice(const std::string& deviceName);

    /// @brief Remove the device name from allow list or block list or both.
    /// @param deviceName the name of the device
    /// @return true on success
    bool removeBvcDevice(const std::string& deviceName);

    /// @brief Check if the device will be allowed with Krisp BVC tech.
    /// @param deviceName the name of the device
    /// @return true if allowed with BVC tech, false otherwise
    bool isBvcAllowed(const std::string& deviceName);

    /// @brief Allow BVC for unknown devices that are not in the allow list and block list, by
    /// default unknown devices are not enabled with Krisp BVC tech.
    /// @param[in] force set true to allow, false otherwise
    void forceBvc(bool force);

private:
    InternalAudioProcessorBuilder* _audioProcessorBuilderPtr;
};

} // namespace KrispVoiceSdk
#endif
