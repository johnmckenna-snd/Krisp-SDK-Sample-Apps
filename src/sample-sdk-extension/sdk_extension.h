#pragma once

enum SamplingRate { 8k, 16k, 32k, 44k, 48k };


class Session {
public:
	Session(const Session &) = default;
	Session(SamplingRate r, bool is_bvc) :
		m_sampling_rate(r), m_is_bvc(is_bvc) {
	}

	void clean_frame_pcm16(int16_t * frame_ptr);
	void clean_frame_float(float * frame_ptr);

	SamplingRate get_sampling_rate() const {
		return m_sampling_rate;
	}

private:
	SamplingRate m_sampling_rate;
	bool m_is_bvc;
};


class SessionFactory {
public:
	Session create(const std::string & device, SamplingRate rate, bool try_bvc);

private:
	virtual Session impl_create(const std::string & device, SamplingRate rate,
		bool try_bvc) = 0;
};
