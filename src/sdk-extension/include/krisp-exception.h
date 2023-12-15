#ifndef KRISP_EXCEPTIONS_H
#define KRISP_EXCEPTIONS_H

#include <exception>
#include <string>

namespace KrispVoiceSdk
{

class KrispException : public std::exception
{
public:
    KrispException(const std::string& err_msg);
    virtual const char* what() const noexcept override;
private:
    std::string m_error_msg;
};

class KrispModelLoadError : public KrispException
{
public:
    KrispModelLoadError(const std::string& err_msg);
};

class KrispModelSelectionError : public KrispException
{
public:
    KrispModelSelectionError(const std::string& err_msg);
};

class KrispDeviceListError : public KrispException
{
public:
    KrispDeviceListError(const std::string& err_msg);
};

class KrispModelScannerError : public KrispException
{
public:
    KrispModelScannerError(const std::string& err_msg);
};

} // namespace KrispVoiceSDK

#endif
