#pragma once

#include <string>
#include <set>


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
		explicit ConstIterator(TContainer::const_iterator iter) : it(iter) {}

		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		bool operator == (const ConstIterator & other) const {
			return it == other.it;
		}
		bool operator != (const ConstIterator & other) const {
			return it != other.it;
		}
		const std::string & operator * () {
			return *it;
		}
		ConstIterator & operator ++ () {
			++it;
			return *this;
		}
	};

	using iterator = ConstIterator;
	using const_iterator = ConstIterator;

	ConstIterator begin() const
	{
		return ConstIterator(m_devices.cbegin());
	}

	ConstIterator end() const
	{
		return ConstIterator(m_devices.cend());
	}

	size_t size() const
	{
		return m_devices.size();
	}

	const TContainer & get_container_ref() const
	{
		return m_devices;
	}
	//using size_type = size_t;
	//std::pair<iterator, bool> insert(const std::string & device);
	//size_type count(const std::string & device) const;
	//size_type erase(const std::string & device);
	//bool dump_to_file();

	void create_empty_file(const std::string & path);
	void load_from_file(const std::string & path);
	bool add(const std::string & device);
	bool remove(const std::string & device);
	bool is_in_the_list(const std::string & device) const;
	unsigned long count() const;

private:
	std::string m_file_path;
	TContainer m_devices;
};

}
