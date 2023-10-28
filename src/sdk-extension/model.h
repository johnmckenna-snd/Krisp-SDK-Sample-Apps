#pragma once

#include <string>


namespace KrispAudioSDK
{


class Model
{
public:
	Model();
	~Model();
	bool load(const std::wstring & path, const std::string & given_name);
	bool load(void * blob_ptr, const std::string & given_name);
	bool unload();
	std::string get_given_name() const;
	bool is_loaded() const;
	const std::string & get_last_error() const;
	bool has_error() const;
	std::string pull_last_error();
private:
	std::string m_given_name;
	std::string m_last_error;
	bool m_loaded;
};

}
