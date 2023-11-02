#include <iostream>


typedef SampleType short;

class FrameSequenceFrameSizeResizer {
public:
	FrameSequenceFrameSizeResizer(
		unsigned samplingRate,
		unsigned inputFrameSizeMilliseconds,
		unsigned outputFrameSizeMilliseconds) :
		mInputFrameSize(samplingRate * inputFrameSizeMilliseconds / 1000),
		mOutputFrameSize(samplingRate * outputFrameSizeMilliseconds / 1000),
		mFrameRemainderPtr(nullptr),
		mFrameRemainderEndPtr(0),
		mFramePtr(nullptr),
		mFrameEndPtr(nullptr),
		mFramesTaken(0) {
		if (mInputFrameSize <= mOutputFrameSize) {
			assert(0);
		}
		//	mNumberToTake = mInputFrameSize / mOutputFrameSize;
	}

	bool frameAvailable() const {
	}

	void feedFrame(SampleType * framePtr) {
		mFramePtr = framePtr;
		mFrameEndPtr = mFramePtr + mInputFrameSize;
	}

	SampleType * takeFrame() {
		SampleType * framePtr;
		if (mFrameRemainderPtr) {
			framePtr = mFrameRemainderPtr;
			unsigned remainderSize = mFrameRemainderEndPtr - mFrameRemainderPtr;
			mFramePtr += mInputFrameSize - remainderSize;
			mFrameRemainderPtr = nullptr;
			mFrameRemainderEndPtr = nullptr;
		}
		else {
			framePtr = mFramePtr;
			if (mFramePtr + mInputFrameSize <= mFrameEndPtr) {
				mFramePtr += mInputFrameSize;
			}
			else {
				mFrameRemainderPtr = mFramePtr;
				mFrameRemainderEndPtr = mFrameEndPtr;
			}
		}
		return framePtr;
	}

private:
	unsigned mInputFrameSize;
	unsigned mOutputFrameSize;
	SampleType * mFrameRemainderPtr;
	unsigned mFrameRemainderSize;
	SampleType * mCurrentFramePtr;
	unsigned mFramesTaken;
	std::vector<SampleType> mStitchingFrame;
};


void testFrameSequenceFrameSizeResizer(
	unsigned samplingRate,
	unsigned inputFrameSizeMilliseconds,
	unsigned outputFrameSizeMilliseconds)
{
	unsigned mSamplingRate;
	unsigned mAudioStreamSize;
	unsigned mInputFrameSizeMilliseconds;
	unsigned mOutputFrameSizeMilliseconds;

	std::vector<SampleType> audioStream;

	unsigned inputFrameSize =
		mSamplingRate * inputFrameSizeMilliseconds / 1000;
	unsigned outputFrameSize =
		mSamplingRate * outputFrameSizeMilliseconds / 1000;

	constexpr unsigned frameCount = 100;
	unsigned audioStreamSize = frameCount * inputFrameSize;
	if (outputFrameSize > inputFrameSize) {
		audioStreamSize =
			(audioStreamSize * outputFrameSize) / inputFrameSize;
		audioStreamSize -= audioStreamSize % inputFrameSize;
	}
	mAudioStreamSize.reserve(audioStreamSize);
	mAudioStreamSize.resize(audioStreamSize);

	FrameSequenceFrameSizeResizer fsqResizer(
		mSamplingRate,
		mInputFrameSizeMilliseconds,
		mOutputFrameSizeMilliseconds
	);

	for(unsigned i = 0; i < audioStream.size(); i += inputFrameSize) {
		fsqResizer.feedFrame(audioStream.data[i]);
		while (fsqResizer.frameAvailable()) {
			auto framePtr = fsqResizer.takeFrame();
			processFrame(framePtr);
		}

	}
}

int main() {
	std::cout << "Hello" << std::endl;
	testFrameSequenceFrameSizeResizer(48000, 32, 10);
}
