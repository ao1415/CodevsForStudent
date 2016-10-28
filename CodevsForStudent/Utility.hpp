#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <functional>
#include <set>
#include <cassert>
#include <cmath>

using namespace std;

const int StageWidth = 10;
const int StageHeight = 16;
const int PackSize = 3;
const int AddScore = 10;

const int EmptyBlock = 0;
const int ObstacleBlock = 11;

const int Rotation = 4;

class Hash {
public:

	using Type = unsigned long long int;

	static const Type FNV(const void* data, const size_t size) {
		const Type offset_basis = 2166136261;
		const Type FNV_prime = 16777619;

		Type hash = offset_basis;

		for (size_t i = 0; i < size; i++)
			hash = (hash*FNV_prime) ^ (((char*)data)[i]);

		return hash;
	}
	static const Type FNVa(const void* data, const size_t size) {
		const Type offset_basis = 2166136261;
		const Type FNV_prime = 16777619;

		Type hash = offset_basis;

		for (size_t i = 0; i < size; i++)
			hash = (hash ^ (((char*)data)[i])) * FNV_prime;

		return hash;
	}

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

inline const int score2obstacle(const int score) { return score / 5; }
