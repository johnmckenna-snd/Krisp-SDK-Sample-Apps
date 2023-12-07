#include <iostream>
#include <string>
#include <vector>

#include <krisp-audio-sdk-nc.hpp>
#include <krisp-audio-sdk-nc-stats.hpp>

#include "argument_parser.hpp"
#include "sound_file.hpp"

#include "krisp-exception.h"
#include "krisp-audio-sdk-ext.h"


std::pair<bool, std::string> WriteFramesToFile(
	const std::string &fileName,
	const std::vector<int16_t> &frames,
	unsigned samplingRate)
{
	return writeSoundFilePCM16(fileName, frames, samplingRate);
}

std::pair<bool, std::string> WriteFramesToFile(
	const std::string &fileName,
	const std::vector<float> &frames,
	unsigned samplingRate)
{
	return writeSoundFileFloat(fileName, frames, samplingRate);
}

template <typename T>
int error(const T &e)
{
	std::cerr << e << std::endl;
	return 1;
}

void readAllFrames(const SoundFile &sndFile, std::vector<short> &frames)
{
	sndFile.readAllFramesPCM16(&frames);
}

void readAllFrames(const SoundFile &sndFile, std::vector<float> &frames)
{
	sndFile.readAllFramesFloat(&frames);
}

std::pair<KrispVoiceSDK::SamplingRate, bool> getKrispSamplingRate(unsigned rate)
{
	std::pair<KrispVoiceSDK::SamplingRate, bool> result;
	result.second = true;
	switch (rate)
	{
	case 8000:
		result.first = KrispVoiceSDK::SamplingRate::Sr8000;
		break;
	case 16000:
		result.first = KrispVoiceSDK::SamplingRate::Sr16000;
		break;
	case 32000:
		result.first = KrispVoiceSDK::SamplingRate::Sr32000;
		break;
	case 44100:
		result.first = KrispVoiceSDK::SamplingRate::Sr44100;
		break;
	case 48000:
		result.first = KrispVoiceSDK::SamplingRate::Sr48000;
		break;
	case 88200:
		result.first = KrispVoiceSDK::SamplingRate::Sr88200;
		break;
	case 96000:
		result.first = KrispVoiceSDK::SamplingRate::Sr96000;
		break;
	default:
		result.first = KrispVoiceSDK::SamplingRate::Sr16000;
		result.second = false;
	}
	return result;
}

template <typename SamplingFormat>
int cleanSndFile(
	const SoundFile &inSndFile,
	const std::string &output,
	const std::string &weight_dir,
	bool withStats)
{
	std::vector<SamplingFormat> wavDataIn;
	std::vector<SamplingFormat> wavDataOut;
	readAllFrames(inSndFile, wavDataIn);
	if (inSndFile.getHasError())
	{
		return error(inSndFile.getErrorMsg());
	}
	unsigned samplingRateInt = inSndFile.getHeader().getSamplingRate();
	auto samplingRateResult = getKrispSamplingRate(samplingRateInt);
	if (!samplingRateResult.second)
	{
		return error("Unsupported sample rate");
	}
	KrispVoiceSDK::SamplingRate samplingRate = samplingRateResult.first;

	try
	{
		KrispVoiceSDK::registerModelsDirectory(weight_dir);
	}
	catch (const KrispVoiceSDK::KrispModelScannerError & err)
	{
		return error(err.what());
	}

	using KrispVoiceSDK::AudioProcessor;
	std::unique_ptr<AudioProcessor> frame_cleaner_ptr;

	try
	{
		frame_cleaner_ptr = KrispVoiceSDK::createNc(samplingRate);
	}
	catch (const KrispVoiceSDK::KrispException & err)
	{
		return error(err.what());
	}
	if (!frame_cleaner_ptr.get())
	{
		return error("null pointer from the factory");
	}
	size_t frameSize = frame_cleaner_ptr->getFrameSize();

	wavDataOut.resize(wavDataIn.size());
	size_t i;
	for (i = 0; (i + 1) * frameSize <= wavDataIn.size(); ++i)
	{
		bool success = frame_cleaner_ptr->processFrame(
			&wavDataIn[i * frameSize],
			&wavDataOut[i * frameSize]);
		if (!success)
		{
			return error("Error processing frame.");
		}
	}
	wavDataOut.resize(i * frameSize);
	auto pairResult = WriteFramesToFile(output, wavDataOut, samplingRate);
	if (!pairResult.first)
	{
		return error(pairResult.second);
	}
	return 0;
}

int processWavFile(const std::string &input, const std::string &output,
		const std::string &weight_dir, bool withStats)
{
	SoundFile inSndFile;
	inSndFile.loadHeader(input);
	if (inSndFile.getHasError())
	{
		return error(inSndFile.getErrorMsg());
	}
	auto sndFileHeader = inSndFile.getHeader();
	if (sndFileHeader.getFormat() == SoundFileFormat::PCM16)
	{
		return cleanSndFile<short>(inSndFile, output, weight_dir, withStats);
	}
	if (sndFileHeader.getFormat() == SoundFileFormat::FLOAT)
	{
		return cleanSndFile<float>(inSndFile, output, weight_dir, withStats);
	}
	return error("The sound file format should be PCM16 or FLOAT.");
}

bool parseArguments(std::string &input, std::string &output,
					std::string &weight_dir, bool &stats, int argc, char **argv)
{
	ArgumentParser p(argc, argv);
	p.addArgument("--input", "-i", IMPORTANT);
	p.addArgument("--output", "-o", IMPORTANT);
	p.addArgument("--weight_dir", "-w", IMPORTANT);
	p.addArgument("--stats", "-s", OPTIONAL);
	if (p.parse())
	{
		input = p.getArgument("-i");
		output = p.getArgument("-o");
		weight_dir = p.getArgument("-w");
		stats = p.getOptionalArgument("-s");
	}
	else
	{
		std::cerr << p.getError();
		return false;
	}
	return true;
}

int main(int argc, char **argv)
{
	std::string in, out, weight_dir;
	bool stats = false;
	if (parseArguments(in, out, weight_dir, stats, argc, argv))
	{
		return processWavFile(in, out, weight_dir, stats);
	}
	else
	{
		std::cerr << "\nUsage:\n\t" << argv[0]
				  << " -i input.wav -o output.wav -w weightDir" << std::endl;
		if (argc == 1)
		{
			return 0;
		}
		return 1;
	}
}
