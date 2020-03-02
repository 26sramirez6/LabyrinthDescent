#pragma once
#include <ctime>
#include <random>

class Frng {
public:
	static Frng getInstance() {
		static Frng instance;
		return instance;
	}

private:
	Frng() : m_engine(m_seed()) {
		srand(static_cast <unsigned> (time(0)));
	}

public:
	Frng(Frng const&) = delete;
	void operator=(Frng const&) = delete;

	inline float simple(const float _min, const float _max) {
		return _min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_max - _min)));
	}

	inline float uniform(const float _min, const float _max) {
		std::uniform_real<float> dist(_min, _max);
		return dist(m_engine);
	}

	std::random_device m_seed;
	std::mt19937 m_engine;
	std::uniform_real<float> m_dist;
};