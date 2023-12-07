#pragma once

#include "device_list.h"


namespace KrispVoiceSDK
{


class BVCDeviceManager
{
public:
	BVCDeviceManager();
	void load_lists(const std::string & allow_list_path,
		const std::string & block_list_path);
	bool allow_device(const std::string & device);
	bool block_device(const std::string & device);
	bool remove_device(const std::string & device);
	bool is_allowed(const std::string & device) const;
	void force_bvc(bool enforce_bvc);

private:
	DeviceList m_allow_list;
	DeviceList m_block_list;
	bool m_enforce_bvc;
};

}
