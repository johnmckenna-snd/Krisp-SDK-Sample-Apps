#pragma once

#include <string>
#include <set>


namespace KrispAudioSDK {

class DeviceList {
public:
	bool load_from_file(const std::string & path);
	bool add(const std::string & device);
	bool remove(const std::string & device);
	bool is_in_the_list(const std::string & device) const;

	bool has_error() const;
	std::string get_last_error() const;
	std::string pull_last_error();
private:
	std::string m_last_error;
	std::string m_file_path;
	std::set<std::string> m_devices;
};

}
