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

	const long long hours() const { return std::chrono::duration_cast<std::chrono::hours>(e - s).count(); }
	const long long minutes() const { return std::chrono::duration_cast<std::chrono::minutes>(e - s).count(); }
	const long long second() const { return std::chrono::duration_cast<std::chrono::seconds>(e - s).count(); }
	const long long millisecond() const { return std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count(); }
	const long long microseconds() const { return std::chrono::duration_cast<std::chrono::microseconds>(e - s).count(); }
	const long long nanoseconds() const { return std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count(); }

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	std::chrono::time_point<std::chrono::high_resolution_clock> e;

};

class Timer {
public:

	Timer() = default;
	Timer(const std::chrono::nanoseconds& _time) {
		type = Type::nanoseconds;
		time = _time.count();
	}
	Timer(const std::chrono::microseconds& _time) {
		type = Type::microseconds;
		time = _time.count();
	}
	Timer(const std::chrono::milliseconds& _time) {
		type = Type::milliseconds;
		time = _time.count();
	}
	Timer(const std::chrono::seconds& _time) {
		type = Type::seconds;
		time = _time.count();
	}
	Timer(const std::chrono::minutes& _time) {
		type = Type::minutes;
		time = _time.count();
	}
	Timer(const std::chrono::hours& _time) {
		type = Type::hours;
		time = _time.count();
	}

	void set(const std::chrono::nanoseconds& _time) {
		type = Type::nanoseconds;
		time = _time.count();
	}
	void set(const std::chrono::microseconds& _time) {
		type = Type::microseconds;
		time = _time.count();
	}
	void set(const std::chrono::milliseconds& _time) {
		type = Type::milliseconds;
		time = _time.count();
	}
	void set(const std::chrono::seconds& _time) {
		type = Type::seconds;
		time = _time.count();
	}
	void set(const std::chrono::minutes& _time) {
		type = Type::minutes;
		time = _time.count();
	}
	void set(const std::chrono::hours& _time) {
		type = Type::hours;
		time = _time.count();
	}

	void start() { s = std::chrono::high_resolution_clock::now(); }

	inline const bool check() const {
		const auto e = std::chrono::high_resolution_clock::now();
		switch (type)
		{
		case Type::nanoseconds:
		{
			const auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(e - s);
			return d.count() >= time;
		}
		case Type::microseconds:
		{
			const auto d = std::chrono::duration_cast<std::chrono::microseconds>(e - s);
			return d.count() >= time;
		}
		case Type::milliseconds:
		{
			const auto d = std::chrono::duration_cast<std::chrono::milliseconds>(e - s);
			return d.count() >= time;
		}
		case Type::seconds:
		{
			const auto d = std::chrono::duration_cast<std::chrono::seconds>(e - s);
			return d.count() >= time;
		}
		case Type::minutes:
		{
			const auto d = std::chrono::duration_cast<std::chrono::minutes>(e - s);
			return d.count() >= time;
		}
		case Type::hours:
		{
			const auto d = std::chrono::duration_cast<std::chrono::hours>(e - s);
			return d.count() >= time;
		}
		default:
			break;
		}
		return true;
	}

	operator bool() const { return check(); }

private:

	enum class Type {
		nanoseconds,
		microseconds,
		milliseconds,
		seconds,
		minutes,
		hours
	};

	std::chrono::time_point<std::chrono::high_resolution_clock> s;
	long long time;
	Type type;

};
