#pragma once

#include <exception>
#include <string>


namespace KrispAudioSDK
{

class KrispException : public std::exception
{
public:
    KrispException(const std::string & err_msg) : m_error_msg(err_msg)
    {
    }

    virtual const char* what() const noexcept override
    {
        return m_error_msg.c_str();
    }

private:
    std::string m_error_msg;
};


class KrispModelLoadError : public KrispException
{
public:
    KrispModelLoadError(const std::string & err_msg) : KrispException(err_msg)
    {
    }
};

class KrispModelSelectionError : public KrispException
{
public:
    KrispModelSelectionError(const std::string & err_msg) : KrispException(err_msg)
    {
    }
};

class KrispDeviceListError : public KrispException
{
public:
    KrispDeviceListError(const std::string & err_msg) : KrispException(err_msg)
    {
    }
};

class KrispModelScannerError : public KrispException
{
public:
    KrispModelScannerError(const std::string & err_msg) : KrispException(err_msg)
    {
    }
};

}
