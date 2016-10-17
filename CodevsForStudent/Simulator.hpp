#pragma once

#include "Share.hpp"

class Simulator {
public:

	//�u���b�N����(�l�n��)
	StageArray cfall(const StageArray& stage) const {

		StageArray next = stage;

		for (int x = 0; x < next.getWidth(); x++)
		{
			int index = next.getHeight() - 1;
			for (int y = next.getHeight() - 1; y >= 0; y--)
			{
				if (next[y][x] != EmptyBlock)
				{
					if (index != y)
					{
						next[index][x] = next[y][x];
						next[y][x] = EmptyBlock;
					}
					index--;
				}
			}
		}

		return next;
	}
	//�u���b�N����(�Q�Ɠn��)
	void fall(StageArray& stage) const {

		for (int x = 0; x < stage.getWidth(); x++)
		{
			int index = stage.getHeight() - 1;
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] != EmptyBlock)
				{
					if (index != y)
					{
						stage[index][x] = stage[y][x];
						stage[y][x] = EmptyBlock;
					}
					index--;
				}
			}
		}
	}

	//�S�Ă̏����I��(�l�n��)
	StageArray cnext(const StageArray& stage, int& s) const {

		StageArray next = stage;

		int score = 0;

		int chain = 1;
		int count = 0;
		fall(next);
		while ((count = disBlocks(next)) > 0)
		{
			score += (int)pow(1.3, chain)*(int)(count / 2);
			chain++;
			fall(next);
		}

		s = score;

		return next;
	}
	void next(StageArray& stage, int& s) const {

		int score = 0;

		int chain = 1;
		int count = 0;
		fall(stage);
		while ((count = disBlocks(stage)) > 0)
		{
			score += (int)pow(1.3, chain)*(int)(count / 2);
			chain++;
			fall(stage);
		}

		s = score;

	}

private:

	const int disBlocks(StageArray& stage) const {

		StageArray next = stage;

		int count = 0;

		//�E, ����, ��, �E��
		const Point direction[] = { Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		for (int y = 0; y < stage.getHeight(); y++)
		{
			for (int x = 0; x < stage.getWidth(); x++)
			{
				if (stage[y][x] == 0) continue;

				//����
				for (const auto& dire : direction)
				{
					int add = stage[y][x];
					Point p = Point(x, y);

					//�T��
					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;
						if (!inside(p) || stage[p] == 0) break;
						add += stage[p];

						//10�ɂȂ���
						if (add == AddScore)
						{
							for (int j = i; j >= 0; j--)
							{
								next[p] = 0;
								count++;
								p -= dire;
							}
							break;
						}
						else if (add > AddScore)
							break;
					}
				}
			}
		}

		stage = move(next);

		return count;
	}

};
