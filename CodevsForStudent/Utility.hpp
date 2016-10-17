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

using namespace std;

const int StageWidth = 10;
const int StageHeight = 16;
const int PackSize = 3;
const int AddScore = 10;

const int EmptyBlock = 0;

using PackArray = std::array<std::array<int, PackSize>, PackSize>;

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

class Pack {
public:

	Pack() = default;
	Pack(std::array<PackArray, 4>&& p) {
		blocks = move(p);

		for (int i = 0; i < 4; i++)
		{
			int minSide = PackSize;
			int maxSide = 0;

			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{
					if (blocks[i][y][x] != EmptyBlock)
					{
						minSide = min(minSide, x);
						maxSide = max(maxSide, x);
					}
				}
			}

			side[i] = { minSide,maxSide };
		}
	}
	Pack(Pack&& p) {
		blocks = move(p.blocks);
		side = move(p.side);
	}

	Pack(const Pack& p) = default;

	static Pack input() {
		std::array<PackArray, 4> data;

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				int block;
				cin >> block;
				data[0][y][x] = block;
			}
		}

		string endStr;
		cin >> endStr;

		for (int r = 1; r < 4; r++)
		{
			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{
					data[r][x][PackSize - 1 - y] = data[r - 1][y][x];
				}
			}
		}

		return Pack(move(data));
	}

	void show() const {
		for (int i = 0; i < 4; i++)
		{
			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{
					cerr << setw(2) << setfill(' ') << blocks[i][y][x] << ",";
				}
				cerr << endl;
			}
			cerr << "---------" << endl;
		}
		cerr << endl;
	}

	const std::array<PackArray, 4>& getArray() const { return blocks; }
	const PackArray& getArray(const int n) const { return blocks[n]; }
	const array<pair<int, int>, 4>& getSide() const { return side; }
	const pair<int, int>& getSide(const int n) const { return side[n]; }

private:

	std::array<PackArray, 4> blocks;
	array<pair<int, int>, 4> side;

};

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Size& s) { return os << CharType('(') << s.width << CharType(', ') << s.height << CharType(')'); }

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Point& p) { return os << CharType('(') << p.x << CharType(', ') << p.y << CharType(')'); }

//inline const bool inside(const Point& p) { return (0 <= p.x && p.x < StageWidth && 0 <= p.y && p.y < StageHeight); }

//‚‚³‚ª+3‚³‚ê‚Ä‚¢‚é‚Ì‚Å’ˆÓ
inline const bool inside(const Point& p) { return (0 <= p.x && p.x < StageWidth && 0 <= p.y && p.y < StageHeight + 3); }

inline const string toCommand(int pos, int rota) { return std::to_string(pos) + " " + std::to_string(rota); }
