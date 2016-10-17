#pragma once

#include "Utility.hpp"

template<typename Type, size_t _Width, size_t _Height>
class CGrid {
public:

	CGrid() { this->fill(Type()); }
	CGrid(const Type& val) { this->fill(val); }

	CGrid(const CGrid<Type, _Width, _Height>& g) = default;
	CGrid(CGrid<Type, _Width, _Height>&& g) noexcept {
		data = std::move(g.data);
	}

	CGrid<Type, _Width, _Height>& operator=(const CGrid<Type, _Width, _Height>& other) = default;
	CGrid<Type, _Width, _Height>& operator=(CGrid<Type, _Width, _Height>&& other) = default;

	bool isBounds(size_t y, size_t x) const noexcept {
		return (0 <= y && y < _Height && 0 <= x && x < _Width);
	}

	Type& at(size_t y, size_t x) {
		if (!isBounds(y, x)) throw std::out_of_range("Grid::at");
		return data[y*_Width + x];
	}
	const Type& at(size_t y, size_t x) const {
		if (!isBounds(y, x)) throw std::out_of_range("Grid::at");
		return data[y*_Width + x];
	}
	Type& at(const Point& pos) { return at(pos.y, pos.x); }
	const Type& at(const Point& pos) const { return at(pos.y, pos.x); }

	Type* operator[](size_t index) { return &data[index*_Width]; }
	Type& operator[](const Point& pos) { return data[pos.y*_Width + pos.x]; }

	const Type* operator[](size_t index) const { return &data[index*_Width]; }
	const Type& operator[](const Point& pos) const { return data[pos.y*_Width + pos.x]; }

	const bool isEmpty() const { return data.empty(); }
	const int getWidth() const { return static_cast<int>(_Width); }
	const int getHeight() const { return static_cast<int>(_Height); }

	void clear() noexcept {
		data.clear();
		_Width = _Height = 0;
	}

	Size size() const noexcept { return Size(static_cast<int>(_Width), static_cast<int>(_Height)); }

	size_t num_elements() const noexcept { return data.size(); }

	void fill(const Type& val) { std::fill(data.begin(), data.end(), val); }

	void show() const {

		for (int y = 0; y < _Height; y++)
		{
			for (int x = 0; x < _Width; x++)
			{
				if (data[y*_Width + x] != 0)
					cerr << setw(2) << setfill(' ') << data[y*_Width + x] << ",";
				else
					cerr << "  ,";
			}
			cerr << endl;
		}
		cerr << endl;
	}

private:

	std::array<Type, _Width*_Height> data;

};
