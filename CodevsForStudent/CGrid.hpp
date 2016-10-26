#pragma once

#include "Utility.hpp"

template<typename Type, size_t _Width, size_t _Height>
class CGrid {
private:

	using MyType = CGrid<Type, _Width, _Height>;
	using ContainerType = std::array<Type, _Width*_Height>;
	using pointer = typename ContainerType::pointer;
	using const_pointer = typename ContainerType::const_pointer;

public:

	CGrid() { this->fill(Type()); }
	CGrid(const Type& val) { this->fill(val); }

	CGrid(const MyType& g) = default;
	CGrid(MyType&& g) noexcept {
		m_data = std::move(g.m_data);
	}

	MyType& operator=(const MyType& other) = default;
	MyType& operator=(MyType&& other) = default;

	bool isBounds(size_t y, size_t x) const noexcept {
		return (0 <= y && y < _Height && 0 <= x && x < _Width);
	}

	Type& at(size_t y, size_t x) {
		if (!isBounds(y, x)) throw std::out_of_range("Grid::at");
		return m_data[y*_Width + x];
	}
	const Type& at(size_t y, size_t x) const {
		if (!isBounds(y, x)) throw std::out_of_range("Grid::at");
		return m_data[y*_Width + x];
	}
	Type& at(const Point& pos) { return at(pos.y, pos.x); }
	const Type& at(const Point& pos) const { return at(pos.y, pos.x); }

	Type* operator[](size_t index) { return &m_data[index*_Width]; }
	Type& operator[](const Point& pos) { return m_data[pos.y*_Width + pos.x]; }

	const Type* operator[](size_t index) const { return &m_data[index*_Width]; }
	const Type& operator[](const Point& pos) const { return m_data[pos.y*_Width + pos.x]; }

	constexpr const bool isEmpty() const { return m_data.empty(); }
	constexpr const int getWidth() const { return static_cast<int>(_Width); }
	constexpr const int getHeight() const { return static_cast<int>(_Height); }

	void clear() noexcept {
		m_data.clear();
		_Width = _Height = 0;
	}

	Size size() const noexcept { return Size(static_cast<int>(_Width), static_cast<int>(_Height)); }

	size_t num_elements() const noexcept { return m_data.size(); }

	void fill(const Type& val) { std::fill(m_data.begin(), m_data.end(), val); }

	void show() const {

		for (int y = 0; y < _Height; y++)
		{
			for (int x = 0; x < _Width; x++)
			{
				if (m_data[y*_Width + x] != 0)
					cerr << setw(2) << setfill(' ') << m_data[y*_Width + x] << ",";
				else
					cerr << "  ,";
			}
			cerr << endl;
		}
		cerr << endl;
	}

	pointer data() { return &m_data[0]; }
	const_pointer data() const { return m_data.data(); }

	//’x‚©‚Á‚½
	//void swap(MyType& other) { m_data.swap(other.m_data); }

private:

	ContainerType m_data;

};
