#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <queue>
#include <functional>
#include <set>

using namespace std;

const int StageWidth = 10;
const int StageHeight = 16;
const int PackSize = 3;
const int AddScore = 10;

const int EmptyBlock = 0;
const int ObstacleBlock = 11;

const int Rotation = 4;

class Stopwatch {
public:

	void start() {
		s = chrono::high_resolution_clock::now();
		e = s;
	}
	void stop() {
		e = chrono::high_resolution_clock::now();
	}

	const long long second() const { return chrono::duration_cast<chrono::seconds>(e - s).count(); }
	const long long millisecond() const { return chrono::duration_cast<chrono::milliseconds>(e - s).count(); }
	const long long microseconds() const { return chrono::duration_cast<chrono::microseconds>(e - s).count(); }

private:

	chrono::time_point<chrono::high_resolution_clock> s;
	chrono::time_point<chrono::high_resolution_clock> e;

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

	void start() { s = chrono::high_resolution_clock::now(); }

	inline const bool check() const {
		const auto e = chrono::high_resolution_clock::now();
		long long t = 0;
		switch (type)
		{
		case Second:
			t = chrono::duration_cast<chrono::seconds>(e - s).count();
			break;
		case MilliSecond:
			t = chrono::duration_cast<chrono::milliseconds>(e - s).count();
			break;
		case MicroSecond:
			t = chrono::duration_cast<chrono::microseconds>(e - s).count();
			break;
		}
		return t >= time;
	}

	operator bool() const { return check(); }

private:

	chrono::time_point<chrono::high_resolution_clock> s;
	long long time = 0;
	int type;

};

class Hash {
public:

	static const size_t FNV(const char* data, const size_t size) {
		const size_t offset_basis = 2166136261;
		const size_t FNV_prime = 16777619;

		size_t hash = offset_basis;

		for (size_t i = 0; i < size; i++)
			hash = (hash*FNV_prime) ^ (data[i]);

		return hash;
	}
	static const size_t FNVa(const char* data, const size_t size) {
		const size_t offset_basis = 2166136261;
		const size_t FNV_prime = 16777619;

		size_t hash = offset_basis;

		for (size_t i = 0; i < size; i++)
			hash = (hash ^ (data[i])) * FNV_prime;

		return hash;
	}

private:



};

struct Size {

	Size() : Size(0, 0) {}
	Size(int w, int h) : width(w), height(h) {}

	int width;
	int height;
};

struct Point {

	Point() : Point(0, 0) {}
	Point(int _x, int _y) : x(_x), y(_y) {}

	int x;
	int y;

	const Point operator+(const Point& p) const { return Point(x + p.x, y + p.y); }
	const Point operator-(const Point& p) const { return Point(x - p.x, y - p.y); }

	void operator+=(const Point& p) { x += p.x; y += p.y; }
	void operator-=(const Point& p) { x -= p.x; y -= p.y; }

	const std::string toString()const { return std::to_string(x) + " " + std::to_string(y); }

};

inline std::ostream& operator << (ostream& os, const Size& s) { return os << "(" << s.width << ", " << s.height << ")"; }

inline std::ostream& operator << (ostream& os, const Point& p) { return os << "(" << p.x << ", " << p.y << ")"; }

//inline const bool inside(const Point& p) { return (0 <= p.x && p.x < StageWidth && 0 <= p.y && p.y < StageHeight); }

//‚‚³‚ª+3‚³‚ê‚Ä‚¢‚é‚Ì‚Å’ˆÓ
inline const bool inside(const int& x, const int& y) { return (0 <= x && x < StageWidth && 0 <= y && y < StageHeight + 3); }
inline const bool inside(const Point& p) { return (0 <= p.x && p.x < StageWidth && 0 <= p.y && p.y < StageHeight + 3); }

inline const string toCommand(int pos, int rota) { return std::to_string(pos) + " " + std::to_string(rota); }
