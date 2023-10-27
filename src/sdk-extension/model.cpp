#include "model.h"

#include <iostream>
#include <krisp-audio-sdk.hpp>


namespace KrispAudioSDK
{


Model::Model() : m_given_name(), m_last_error(), m_loaded(false)
{
}

Model::~Model()
{
	unload();
}

bool Model::load(const std::wstring & path, const std::string & given_name)
{
	if (m_loaded)
	{
		m_last_error = "Please use Model::unload() first to unload the model.";
		return false;
	}
	int result = krispAudioSetModel(path.c_str(), given_name.c_str());
	if (result != 0)
	{
		m_last_error = "Failed to load the model";
		return false;
	}
	m_loaded = true;
	m_given_name = given_name;
	return true;
}

bool Model::unload()
{
	if (m_loaded)
	{
		int result = krispAudioRemoveModel(m_given_name.c_str());
		if (result !=0)
		{
			return false;
		}
		m_given_name.clear();
		m_loaded = false;
		return true;
	}
	return false;
}

std::string Model::get_given_name() const
{
	return m_given_name;
}

bool Model::is_loaded() const
{
	return m_loaded;
}

const std::string & Model::get_last_error() const
{
	return m_last_error;
}

bool Model::has_error() const
{
	return m_last_error.size() ? true : false;
}

std::string Model::pull_last_error() 
{
	return std::move(m_last_error);
}


}
