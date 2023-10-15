#pragma once

#include <string>
#include <memory>

#include "session.h"


namespace KrispAudioSDK {

class SessionFactory {
public:
	virtual ~SessionFactory() = default;

	std::unique_ptr<Session> create(
		const std::string & device, SamplingRate r, bool try_bvc);
private:
	virtual std::unique_ptr<Session> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc) = 0;
};

}
