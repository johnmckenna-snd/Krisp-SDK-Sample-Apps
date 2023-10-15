#include "session_factory.h"


std::unique_ptr<Session> SessionFactory::create(
	const std::string & device, SamplingRate r, bool try_bvc)
{
	auto session = this->create_impl(device, r, try_bvc);
	return session;
}
