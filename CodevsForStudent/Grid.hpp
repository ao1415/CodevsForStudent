#pragma once

#include "Utility.hpp"

template<typename Type>
class Grid {
public:

	Grid() = default;
	Grid(const size_t w, const size_t h) : data(w*h, Type()), width(w), height(h) {}
	Grid(const Size& size) : Grid(size.width, size.height) {}
	Grid(const size_t w, const size_t h, const Type& val) : data(w*h, val), width(w), height(h) {}
	Grid(const Size& size, const Type& val) : Grid(size.width, size.height, val) {}

	Grid(const Grid& g) = default;
	Grid(Grid&& g) noexcept {
		width = g.width;
		height = g.height;
		data = std::move(g.data);
		g.width = g.height = 0;
	}

	Grid& operator=(const Grid& other) = default;
	Grid& operator=(Grid&& other) = default;

	bool inBounds(size_t y, size_t x) const noexcept {
		return (0 <= y && y < height && 0 <= x && x < width);
	}

	Type& at(size_t y, size_t x) {
		if (!inBounds(y, x)) throw std::out_of_range("Grid::at");
		return data[y*width + x];
	}
	const Type& at(size_t y, size_t x) const {
		if (!inBounds(y, x)) throw std::out_of_range("Grid::at");
		return data[y*width + x];
	}
	Type& at(const Point& pos) { return at(pos.y, pos.x); }
	const Type& at(const Point& pos) const { return at(pos.y, pos.x); }

	Type* operator[](size_t index) { return &data[index*width]; }
	Type& operator[](const Point& pos) { return data[pos.y*width + pos.x]; }

	const Type* operator[](size_t index) const { return &data[index*width]; }
	const Type& operator[](const Point& pos) const { return data[pos.y*width + pos.x]; }

	const bool isEmpty() const { return data.empty(); }
	const int getWidth() const { return static_cast<int>(width); }
	const int getHeight() const { return static_cast<int>(height); }

	void shrink_to_fit() { data.shrink_to_fit(); }

	void release() { clear(); shrink_to_fit(); }

	void reserve(size_t w, size_t h) { data.reserve(w*h); }

	void resize(size_t w, size_t h, const Type& val) {
		if (w == width && h == height) return;
		width = w;
		height = h;
		data.assign(w*h, val);
	}
	void resize(size_t w, size_t h) { resize(w, h, Type()); }
	void resize(const Size& size, const Type& val) { resize(size.width, size.height, val); }
	void resize(const Size& size) { resize(size.width, size.height, Type()); }

	void clear() noexcept {
		data.clear();
		width = height = 0;
	}

	Size size() const noexcept { return Size(static_cast<int>(width), static_cast<int>(height)); }

	size_t num_elements() const noexcept { return data.size(); }

	void fill(const Type& val) { std::fill(data.begin(), data.end(), val); }

private:

	std::vector<Type> data;
	size_t width = 0;
	size_t height = 0;

};
