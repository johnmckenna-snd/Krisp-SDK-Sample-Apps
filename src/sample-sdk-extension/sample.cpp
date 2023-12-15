#include "krisp-voice-sdk.h"

KrispVoiceSdk::SamplingRate getSamplingRateOfTheAudio();

void useCaseSingleStreamModelDir()
{
    KrispVoiceSdk::registerModelsDirectory("/home/user/krisp_models");
    KrispVoiceSdk::SamplingRate rate = getSamplingRateOfTheAudio();
    auto noiseCancellerPtr = KrispVoiceSdk::createNc(rate);
    auto frameSize = noiseCancellerPtr->getFrameSize();
    std::vector<short> inputFrame;
    std::vector<short> outputFrame;
    outputFrame.resize(frameSize);
    for (;;)
    {
        noiseCancellerPtr->processFrame(inputFrame.data(), outputFrame.data());
    }
}

void useCaseSingleStreamExplicitSingleModel()
{
    auto modelPath = L"/home/user/c6.f.s.ced125.kw.thw";
    KrispVoiceSdk::registerModel(KrispVoiceSdk::ModelId::MicNc32K, modelPath);
    auto noiseCancellerPtr = KrispVoiceSdk::createNc(
        KrispVoiceSdk::SamplingRate::Sr32000, KrispVoiceSdk::ModelId::MicNc32K);
    auto frameSize = noiseCancellerPtr->getFrameSize();
    std::vector<short> inputFrame;
    std::vector<short> outputFrame;
    outputFrame.resize(frameSize);
    for (;;)
    {
        noiseCancellerPtr->processFrame(inputFrame.data(), outputFrame.data());
    }
}

void useCaseMultipleModelsSingleStream()
{
    auto fbModelPath = L"/home/user/c6.f.s.ced125.kw.thw";
    auto wbModelPath = L"/home/user/c5.s.w.c9ac8f.kw";
    auto nbModelPath = L"/home/user/c5.n.s.20949d.kw";
    KrispVoiceSdk::registerModel(KrispVoiceSdk::ModelId::MicNc32K, fbModelPath);
    KrispVoiceSdk::registerModel(KrispVoiceSdk::ModelId::MicNc16K, wbModelPath);
    KrispVoiceSdk::registerModel(KrispVoiceSdk::ModelId::MicNc8K, nbModelPath);
    KrispVoiceSdk::SamplingRate rate = getSamplingRateOfTheAudio();
    auto noiseCancellerPtr = KrispVoiceSdk::createNc(rate);
    auto frameSize = noiseCancellerPtr->getFrameSize();
    std::vector<short> outputFrame;
    outputFrame.resize(frameSize);
    for (;;)
    {
        noiseCancellerPtr->processFrame(inputFrame.data(), outputFrame.data());
    }
}

void useCaseMultipleStreamMultipleModels()
{
    auto modelDirectoryPath = "/home/user/krisp_models";
    KrispVoiceSdk::registerModelsDirectory(modelDirectoryPath);
    KrispVoiceSdk::SamplingRate stream1Rate = getSamplingRateOfTheAudio1();
    KrispVoiceSdk::SamplingRate stream2Rate = getSamplingRateOfTheAudio2();
    auto noiseCanceller1Ptr = KrispVoiceSdk::createNc(stream1Rate);
    auto noiseCanceller2Ptr = KrispVoiceSdk::createNc(stream2Rate);
    auto frame1Size = noiseCanceller1Ptr->getFrameSize();
    auto frame2Size = noiseCanceller2Ptr->getFrameSize();
    std::vector<short> inputFrame1, inputFrame2;
    std::vector<short> outputFrame1;
    std::vector<short> outputFrame2;
    outputFrame1.resize(frame1Size);
    outputFrame2.resize(frame2Size);
    for (;;)
    {
        noiseCanceller1Ptr->processFrame(inputFrame1.data(), outputFrame1.data());
        noiseCanceller2Ptr->processFrame(inputFrame2.data(), outputFrame2.data());
    }
}


int main()
{
    KrispVoiceSdk::registerModelsDirectory("/home/user/krisp_models");
    auto noiseCancellerPtr = KrispVoiceSdk::createNc(KrispVoiceSdk::SamplingRate::Sr32000);
    auto frameSize = noiseCancellerPtr->getFrameSize();
    std::vector<short> outputFrame;
    outputFrame.resize(frameSize);
    for (;;)
    {
        noiseCancellerPtr->processFrame(inputFrame.data(), outputFrame.data());
    }
}

int main()
{
    krispAudioGlobalInit(nullptr);
    krispAudioSetModel(L"/home/user/c6.f.s.ced125.kw.thw", "model_name");
    auto sessionId = krispAudioNcCreateSession(
        KRISP_AUDIO_SAMPLING_RATE_32000HZ,
        KRISP_AUDIO_SAMPLING_RATE_32000HZ,
        KRISP_AUDIO_FRAME_DURATION_10MS,
        "model_name");
    auto frameSize = KRISP_AUDIO_FRAME_DURATION_10MS *
                     KRISP_AUDIO_SAMPLING_RATE_32000HZ / 1000;
    std::vector<short> outputFrame;
    outputFrame.resize(frameSize);
    for (;;)
    {
        krispAudioNcCleanAmbientNoise(
            sessionId, inputFrame, frameSize, outputFrame, frameSize);
    }
    krispAudioNCCloseSession(sessionId);
    krispAudioDestroyModel("model_name");
    krispAudioGlobalDestroy();
}
