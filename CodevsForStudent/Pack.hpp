#pragma once

#include "Utility.hpp"
#include "CGrid.hpp"

using PackArray = CGrid<char, PackSize, PackSize>;

class Pack {
public:

	Pack() = default;
	Pack(std::array<PackArray, 4>&& p) {
		blocks = move(p);
		setSide();
	}
	Pack(Pack&& p) {
		blocks = move(p.blocks);
		side = move(p.side);
	}
	Pack(const PackArray& arr) {
		blocks[0] = arr;
		setRota(blocks);
		setSide();
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

		Pack::setRota(data);

		return Pack(move(data));
	}

	Pack getFullObstacle(int& num) const {

		Pack full = *this;

		if (num <= 0)
		{
			full.setSide();
			return full;
		}

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				if (num > 0 && blocks[0][y][x] == EmptyBlock)
				{
					full.blocks[0][y][x] = ObstacleBlock;
					num--;
				}
				if (num <= 0)
				{
					y = x = PackSize;
				}
			}
		}

		Pack::setRota(full.blocks);
		full.setSide();

		return full;
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

	array<PackArray, 4> blocks;
	array<pair<int, int>, 4> side;

	void setSide() {

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

	static void setRota(array<PackArray, 4>& data) {

		for (int r = 1; r < Rotation; r++)
		{
			for (int y = 0; y < PackSize; y++)
			{
				for (int x = 0; x < PackSize; x++)
				{
					data[r][x][PackSize - 1 - y] = data[r - 1][y][x];
				}
			}
		}

	}

};
