#include <dirent.h>

#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <map>


#include "krisp-exception.h"
#include "krisp-audio-sdk-ext.h"
#include "audio_processor_builder.h"


namespace KrispVoiceSDK
{


class ModelCollectorFromDirectory
{
public:
	explicit ModelCollectorFromDirectory(
		const std::initializer_list<std::pair<const char *, ModelId>> & models_name_to_id)
		: m_models_name_to_id(), m_models_id_to_path()
	{
		for (auto name_to_id : models_name_to_id)
		{
			m_models_name_to_id.insert(name_to_id);
		}
	}

	void clear()
	{
		m_models_id_to_path.clear();
	}

	void scan_directory(const std::string & directory)
	{
#ifdef WIN32
		constexpr char path_separator[] = "\\";
#else
		constexpr char path_separator[] = "/";
#endif
		DIR * dir = ::opendir(directory.c_str());
		if (!dir)
		{
			throw KrispModelScannerError("Can't access " + directory + ".");
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
			auto it_mic_model = m_models_name_to_id.find(file_name);
			if (it_mic_model != m_models_name_to_id.end())
			{
				auto model_file_name = it_mic_model->first;
				auto model_id = it_mic_model->second;
				std::string path = directory + path_separator + model_file_name;
				m_models_id_to_path[model_id] = path;
				++models_found;
			}
		}
		::closedir(dir);
		if (!models_found)
		{
			throw KrispModelScannerError("No models found in " + directory + ".");
		}
	}

	using ModelId = KrispVoiceSDK::ModelId;

	const std::map<ModelId, std::string> & get_mic_models_paths() const
	{
		return m_models_id_to_path;
	}

private:
	std::map<std::string, ModelId> m_models_name_to_id;
	std::map<ModelId, std::string> m_models_id_to_path;
};


std::vector<ModelId> get_models_from_directory(AudioProcessorBuilder & builder,
	const std::string & directory,
	const ModelNameToIdMap & model_name_to_id_map)
{
	std::vector<ModelId> found_models;
	ModelCollectorFromDirectory model_collector(model_name_to_id_map);
	model_collector.scan_directory(directory);
	found_models.reserve(8);
	auto mic_models = model_collector.get_mic_models_paths();
	for (auto model_id_and_path : mic_models)
	{
		auto model_id = model_id_and_path.first;
		auto model_path = model_id_and_path.second;
		found_models.push_back(model_id);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wstringConverter;
		std::wstring model_wpath = wstringConverter.from_bytes(model_path);
		builder.registerModel(model_id, model_wpath);
	}
	return found_models;
}

}
