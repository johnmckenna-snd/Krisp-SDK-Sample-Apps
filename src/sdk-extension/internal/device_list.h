#pragma once

#include <set>
#include <string>

namespace KrispVoiceSDK
{

class DeviceList
{
public:
    typedef std::set<std::string> TContainer;

    class ConstIterator
    {
        TContainer::const_iterator it;

    public:
        explicit ConstIterator(TContainer::const_iterator iter) : it(iter)
        {
        }

        using iterator_category = std::forward_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        bool operator==(const ConstIterator& other) const
        {
            return it == other.it;
        }
        bool operator!=(const ConstIterator& other) const
        {
            return it != other.it;
        }
        const std::string& operator*()
        {
            return *it;
        }
        ConstIterator& operator++()
        {
            ++it;
            return *this;
        }
    };

    using iterator = ConstIterator;
    using const_iterator = ConstIterator;

    ConstIterator begin() const
    {
        return ConstIterator(_devices.cbegin());
    }

    ConstIterator end() const
    {
        return ConstIterator(_devices.cend());
    }

    size_t size() const
    {
        return _devices.size();
    }

    const TContainer& getContainerRef() const
    {
        return _devices;
    }
    // using size_type = size_t;
    // std::pair<iterator, bool> insert(const std::string & device);
    // size_type count(const std::string & device) const;
    // size_type erase(const std::string & device);
    // bool dump_to_file();

    void createEmptyFile(const std::string& path);
    void loadFromFile(const std::string& path);
    bool add(const std::string& device);
    bool remove(const std::string& device);
    bool isInTheList(const std::string& device) const;
    unsigned long count() const;

private:
    std::string _filePath;
    TContainer _devices;
};

} // namespace KrispVoiceSDK
