#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <algorithm>
#include <iomanip>

using namespace std;

#define Property_Get(type,name) __declspec(property(get=_get_##name))type name;\
type _get_##name()

const int StageWidth = 10;
const int StageHeight = 16;
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

class Pack {
public:

	Pack() = default;
	Pack(PackArray&& p) { blocks = move(p); }
	Pack(Pack&& p) { blocks = move(p.blocks); }

	Pack(const Pack& p) = default;

	static Pack input() {
		PackArray data;

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

private:

	PackArray blocks;

};

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Size& s) { return os << CharType('(') << s.width << CharType(', ') << s.height << CharType(')'); }

template <class CharType>
inline std::basic_ostream<CharType>& operator << (std::basic_ostream<CharType>& os, const Point& p) { return os << CharType('(') << p.x << CharType(', ') << p.y << CharType(')'); }

inline const bool inside(const Point& p) { return (0 <= p.x && p.x < StageWidth && 0 <= p.y && p.y < StageHeight); }
