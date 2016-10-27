#pragma once

#include <chrono>

class Stopwatch {
public:

	void start() {
		s = std::chrono::high_resolution_clock::now();
		e = s;
	}
	void stop() {
		e = std::chrono::high_resolution_clock::now();
	}

	const long long second() const { return std::chrono::duration_cast<std::chrono::seconds>(e - s).count(); }
	const long long millisecond() const { return std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count(); }
	const long long microseconds() const { return std::chrono::duration_cast<std::chrono::microseconds>(e - s).count(); }

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	std::chrono::time_point<std::chrono::high_resolution_clock> e;

};

class Timer {
public:

	static const int Second = 1;
	static const int MilliSecond = 2;
	static const int MicroSecond = 3;

	Timer() = default;
	Timer(const long long _time, const int _type) {
		setTimer(_time, _type);
	}

	void setTimer(const long long _time, const int _type) {
		time = _time;
		type = _type;
	}

	void start() { s = std::chrono::high_resolution_clock::now(); }

	inline const bool check() const {
		const auto e = std::chrono::high_resolution_clock::now();
		long long t = 0;
		switch (type)
		{
		case Second:
			t = std::chrono::duration_cast<std::chrono::seconds>(e - s).count();
			break;
		case MilliSecond:
			t = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
			break;
		case MicroSecond:
			t = std::chrono::duration_cast<std::chrono::microseconds>(e - s).count();
			break;
		}
		return t >= time;
	}

	operator bool() const { return check(); }

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	long long time = 0;
	int type;

};
