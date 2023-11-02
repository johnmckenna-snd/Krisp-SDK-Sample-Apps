#include "helpers.h"

#include <dirent.h>

#include <map>
#include <locale>
#include <codecvt>


namespace KrispAudioSDK
{


class ModelCollectorFromDirectory
{
public:
	void clear()
	{
		m_mic_models_id_to_path.clear();
	}

	bool scan_directory(const std::string & directory)
	{
#ifdef WIN32
		constexpr char path_separator[] = "\\";
#else
		constexpr char path_separator[] = "/";
#endif
		DIR * dir = ::opendir(directory.c_str());
		if (!dir)
		{
			return false;
		}
		struct dirent * dir_item = nullptr;
		unsigned models_found = 0;
		while (true)
		{
			dir_item = ::readdir(dir);
			if (!dir_item)
			{
				break;
			}
			if (dir_item->d_type != DT_REG)
			{
				continue;
			}
			auto file_name = std::string(dir_item->d_name, dir_item->d_namlen);
			auto it_mic_model = m_mic_models_name_to_id.find(file_name);
			if (it_mic_model != m_mic_models_name_to_id.end())
			{
				auto model_file_name = it_mic_model->first;
				auto model_id = it_mic_model->second;
				std::string path = directory + path_separator + model_file_name;
				m_mic_models_id_to_path[model_id] = path;
				++models_found;
			}
		}
		::closedir(dir);
		if (!models_found)
		{
			return false;
		}
		return true;
	}

	using MicModelId = KrispAudioSDK::MicFrameCleanerFactory::ModelId;

	const std::map<MicModelId, std::string> & get_mic_models_paths() const
	{
		return m_mic_models_id_to_path;
	}

private:
	std::map<std::string, MicModelId> m_mic_models_name_to_id =
	{
		{"model-nc-8.kw", MicModelId::nc_8k},
		{"model-nc-16.kw", MicModelId::nc_16k},
		{"model-nc-32.kw", MicModelId::nc_32k},
		{"model-bvc-32.thw", MicModelId::bvc_32k},
		{"c5.n.s.20949d.kw", MicModelId::nc_8k},
		{"c5.s.w.c9ac8f.kw", MicModelId::nc_16k},
		{"c6.f.s.ced125.kw", MicModelId::nc_32k},
		{"hs.c6.f.s.de56df.thw", MicModelId::bvc_32k}
	};

	//std::map<std::string, int> m_speaker_models = 
	//{
	//	{"model-inbound-nc-8.kw", 0},
	//	{"model-inbound-nc-16.kw", 1},
	//	{"c7.n.s.6c5dfc.thw", 0},
	//	{"c7.w.m.011249.thw", 1}
	//};

	std::map<MicModelId, std::string> m_mic_models_id_to_path;
};


bool load_models_from_directory(
	MicFrameCleanerFactory & factory, const std::string & directory)
{
	ModelCollectorFromDirectory model_collector;
	if (!model_collector.scan_directory(directory))
	{
		return false;
	}

	auto mic_models = model_collector.get_mic_models_paths();

	for (auto model_id_and_path : mic_models)
	{
		auto model_id = model_id_and_path.first;
		auto model_path = model_id_and_path.second;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringConverter;
		std::wstring model_wpath = wstringConverter.from_bytes(model_path);
		factory.register_model(model_id, model_wpath);
	}
	return true;
}

}
