#include "bvc_device_manager.h"


namespace KrispVoiceSDK
{

BVCDeviceManager::BVCDeviceManager() : m_allow_list(),
	m_block_list(), m_enforce_bvc(false)
{
}

void BVCDeviceManager::load_lists(const std::string & allow_list_path,
		const std::string & block_list_path)
{
	m_allow_list.load_from_file(allow_list_path);
	m_block_list.load_from_file(block_list_path);
}

bool BVCDeviceManager::allow_device(const std::string & device)
{
	if (m_allow_list.add(device))
	{
		return true;
	}
	return false;
}

bool BVCDeviceManager::block_device(const std::string & device)
{
	if (m_block_list.add(device))
	{
		return true;
	}
	return false;
}

bool BVCDeviceManager::remove_device(const std::string & device)
{
	bool error = false;
	if (!m_allow_list.remove(device)) {
		error = true;
	}
	if (!m_block_list.remove(device)) {
		error = true;
	}
	if (error) {
		return false;
	}
	return true;
}

bool BVCDeviceManager::is_allowed(const std::string & device) const {
	if (m_block_list.is_in_the_list(device))
	{
		return false;
	}
	if (m_enforce_bvc)
	{
		return true;
	}
	if (m_allow_list.is_in_the_list(device))
	{
		return true;
	}
	return false;
}

void BVCDeviceManager::force_bvc(bool enforce_bvc)
{
	m_enforce_bvc = enforce_bvc;
}

}
