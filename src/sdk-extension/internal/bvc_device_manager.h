#pragma once

#include "device_list.h"

namespace KrispVoiceSDK
{

class BVCDeviceManager
{
public:
    BVCDeviceManager();
    void loadLists(const std::string& allowListPath, const std::string& blockListPath);
    bool allowDevice(const std::string& device);
    bool blockDevice(const std::string& device);
    bool removeDevice(const std::string& device);
    bool isAllowed(const std::string& device) const;
    void forceBvc(bool enforceBvc);

private:
    DeviceList _allowList;
    DeviceList _blockList;
    bool _enforceBvc;
};

} // namespace KrispVoiceSDK
