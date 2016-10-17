#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <algorithm>

#define Property_Get(type,name) __declspec(property(get=_get_##name))type name;\
type _get_##name()

const int Width = 10;
const int Height = 16;
const int PackSize = 3;

using PackArray = std::array<std::array<std::array<int, PackSize>, PackSize>, 4>;

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

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Size& s) { return os << CharType('(') << s.width << CharType(', ') << s.height << CharType(')'); }

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Point& p) { return os << CharType('(') << p.x << CharType(', ') << p.y << CharType(')'); }

inline const bool inside(const Point& p) { return (0 <= p.x && p.x < Width && 0 <= p.y && p.y < Height); }
