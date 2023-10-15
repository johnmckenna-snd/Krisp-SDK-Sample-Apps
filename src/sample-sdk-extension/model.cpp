#include "model.h"

#include <krisp-audio-sdk.hpp>


namespace KrispAudioSDK {


Model::Model() : m_given_name(), m_loaded(false)
{
}

Model::~Model()
{
	unload();
}

bool Model::load(const std::wstring & path, const std::string & given_name)
{
	if (m_loaded) {
		return false;
	}

	int result = krispAudioSetModel(path.c_str(), given_name.c_str());
	if (result != 0) {
		return false;
	}
	m_loaded = true;
	m_given_name = given_name;
	return true;
}

bool Model::unload()
{
	if (m_loaded) {
		int result = krispAudioRemoveModel(m_given_name.c_str());
		if (result !=0) {
			return false;
		}
		m_given_name.clear();
		m_loaded = false;
		return true;
	}
	return false;
}

std::string Model::get_given_name() const {
	return m_given_name;
}

bool Model::is_loaded() const {
	return m_loaded;
}

}
