#include "bvc_device_manager.h"


namespace KrispAudioSDK
{

BVCDeviceManager::BVCDeviceManager() : m_errors(), m_allow_list(),
	m_block_list(), m_enforce_bvc(false)
{
	m_errors.reserve(5);
}

void BVCDeviceManager::set_error(const std::string & error)
{
	m_errors.push_back(error);
}

bool BVCDeviceManager::load_lists(const std::string & allow_list_path,
		const std::string & block_list_path)
{
	bool has_error = false;
	if (!m_allow_list.load_from_file(allow_list_path))
	{
		has_error = true;
		set_error(m_allow_list.pull_last_error());
	}
	if (!m_block_list.load_from_file(block_list_path))
	{
		has_error = true;
		set_error(m_block_list.pull_last_error());
	}
	return !has_error;
}

bool BVCDeviceManager::allow_device(const std::string & device)
{
	if (m_allow_list.add(device))
	{
		return true;
	}
	set_error(m_allow_list.pull_last_error());
	return false;
}

bool BVCDeviceManager::block_device(const std::string & device)
{
	if (m_block_list.add(device))
	{
		return true;
	}
	set_error(m_block_list.pull_last_error());
	return false;
}

bool BVCDeviceManager::remove_device(const std::string & device)
{
	bool error = false;
	if (!m_allow_list.remove(device)) {
		error = true;
		set_error(m_allow_list.pull_last_error());
	}
	if (!m_block_list.remove(device)) {
		error = true;
		set_error(m_block_list.pull_last_error());
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
