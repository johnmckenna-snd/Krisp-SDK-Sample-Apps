#pragma once

#include <string>


namespace KrispAudioSDK {

class Model {
public:
	Model();
	~Model();
	bool load(const std::wstring & path, const std::string & given_name);
	bool unload();
	std::string get_given_name() const;
	bool is_loaded() const;
private:
	std::string m_given_name;
	bool m_loaded;
};

}
