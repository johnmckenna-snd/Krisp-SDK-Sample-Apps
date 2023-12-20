#include "bvc_device_manager.h"

namespace KrispVoiceSdk
{

BVCDeviceManager::BVCDeviceManager()
    : _allowList(), _blockList(), _enforceBvc(false)
{
}

void BVCDeviceManager::loadLists(
    const std::string& allow_list_path, const std::string& block_list_path)
{
    _allowList.loadFromFile(allow_list_path);
    _blockList.loadFromFile(block_list_path);
}

bool BVCDeviceManager::allowDevice(const std::string& device)
{
    if (_allowList.add(device))
    {
        return true;
    }
    return false;
}

bool BVCDeviceManager::blockDevice(const std::string& device)
{
    if (_blockList.add(device))
    {
        return true;
    }
    return false;
}

bool BVCDeviceManager::removeDevice(const std::string& device)
{
    bool error = false;
    if (!_allowList.remove(device))
    {
        error = true;
    }
    //if (!_blockList.remove(device))
    //{
    //    error = true;
    //}
    if (error)
    {
        return false;
    }
    return true;
}

bool BVCDeviceManager::isAllowed(const std::string& device) const
{
    if (_blockList.isInTheList(device))
    {
        return false;
    }
    if (_enforceBvc)
    {
        return true;
    }
    if (_allowList.isInTheList(device))
    {
        return true;
    }
    return false;
}

void BVCDeviceManager::forceBvc(bool enforce_bvc)
{
    _enforceBvc = enforce_bvc;
}

} // namespace KrispVoiceSDK
