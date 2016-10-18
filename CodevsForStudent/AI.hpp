#pragma once

#include "Simulator.hpp"

class AI {
public:

	const string think() {
		string com = "0 0";



		return com;
	}

private:

	void setBlocks(StageArray& stage, const PackArray& pack, const int pos) const {

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				if (0 <= x + pos && x + pos < stage.getWidth())
					stage[y][x + pos] = pack[y][x];
			}
		}

	}

	const int eval() const {

		int score = 0;


		return score;
	}

};
