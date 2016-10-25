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
	//���炩���߃u���b�N�𗎂Ƃ��Ă����Ă�������
	StageArray cnext(const StageArray& stage, int& s) const {

		StageArray next = stage;

		int score = 0;

		int chain = 1;
		int count = 0;

		//fall(next);
		while ((count = disBlocks(next)) > 0)
		{
			score += (int)pow(1.3, chain)*(int)(count / 2);
			chain++;
			fall(next);
		}

		s = score;

		return next;
	}
	//�S�Ă̏����I��(�Q�Ɠn��)
	//���炩���߃u���b�N�𗎂Ƃ��Ă����Ă�������
	const int next(StageArray& stage, int& s) const {

		//�L���b�V��������č������ł��邩����
		//std::map<size_t(�X�e�[�W�̃n�b�V���l), std::piar<StageArray(������̃X�e�[�W), vector<int>(�J�E���g�z��)>

		int score = 0;

		int chain = 1;
		int count = 0;

		//fall(stage);
		while ((count = disBlocks(stage)) > 0)
		{
			score += (int)pow(1.3, chain)*(int)(count / 2);
			chain++;
			fall(stage);
		}

		s = score;

		return chain;
	}

	//�Q�[���I�[�o�[�Ȃ��true
	bool isDead(const StageArray& stage) const {

		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < stage.getWidth(); x++)
			{
				if (stage[y][x] != EmptyBlock)
					return true;
			}
		}

		return false;
	}

	StageArray csetBlocks(const StageArray& stage, const PackArray& pack, const int pos) const {

		StageArray next = stage;

		for (int y = 0; y < PackSize; y++)
		{
			for (int x = 0; x < PackSize; x++)
			{
				if (0 <= x + pos && x + pos < stage.getWidth())
					next[y][x + pos] = pack[y][x];
			}
		}

		return next;
	}
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

private:

	const int disBlocks(StageArray& stage) const {

		StageArray next = stage;

		int count = 0;

		//��, �E��, �E, �E��
		const Point direction[] = { Point(0,-1),Point(1,-1),Point(1,0),Point(1,1) };

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0 && stage[y][x] != EmptyBlock; y--)
			{
				//if (stage[y][x] == 0) continue;

				//����
				for (const auto& dire : direction)
				{
					int add = stage[y][x];
					Point p = Point(x, y);

					//�T��
					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;

						if (!inside(p) || stage[p] == EmptyBlock) break;

						add += stage[p];

						//10�ɂȂ���
						if (add == AddScore)
						{
							for (int j = i; j >= 0; j--)
							{
								next[p] = EmptyBlock;
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
