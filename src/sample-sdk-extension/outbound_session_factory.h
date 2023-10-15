#pragma once

#include "session_factory.h"


class OutboundSessionFactory : public SessionFactory {
public:

private:
	std::unique_ptr<Session> create_impl(
		const std::string & device, SamplingRate r, bool try_bvc);
};
