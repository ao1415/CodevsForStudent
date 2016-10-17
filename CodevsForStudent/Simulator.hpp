#pragma once

#include "Share.hpp"

class Simulator {
public:

	//�u���b�N����(�l�n��)
	StageArray cfall(const StageArray& stage) const {

		StageArray next = stage;

		for (int x = 0; x < next.width; x++)
		{
			int index = next.height - 1;
			for (int y = next.height - 1; y >= 0; y--)
			{
				if (next[y][x] != 0)
				{
					if (index != y)
					{
						next[index][x] = next[y][x];
						next[y][x] = 0;
					}
					index--;
				}
			}
		}

		return next;
	}
	//�u���b�N����(�Q�Ɠn��)
	void fall(StageArray& stage) const {

		for (int x = 0; x < stage.width; x++)
		{
			int index = stage.height - 1;
			for (int y = stage.height - 1; y >= 0; y--)
			{
				if (stage[y][x] != 0)
				{
					if (index != y)
					{
						stage[index][x] = stage[y][x];
						stage[y][x] = 0;
					}
					index--;
				}
			}
		}
	}

	//�S�Ă̏����I��(�l�n��)
	StageArray cnext(const StageArray& stage) const {

	}
	void next(StageArray& stage) const {

	}

private:

	const int disBlocks(StageArray& stage) const {

		int count = 0;

		for (int y = 0; y < stage.height; y++)
		{
			for (int x = 0; x < stage.width; x++)
			{

			}
		}

	}

};
