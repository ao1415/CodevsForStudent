#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	const int getScore(const StageArray& _stage, const int _obstacle, const int _attackScore, const int turn) {
		obstacle = _obstacle;
		attackScore = _attackScore;

	}

	void show() const {
		cerr << "���ז��z�u�X�R�A\t:" << obstacleScore << endl;
		cerr << "�����X�R�A\t\t:" << linkScore << endl << endl;

		cerr << "���ʃX�R�A\t\t:" << flatScore << endl;
		cerr << "�u���b�N�̍�������\t:" << average << endl;
		cerr << "U���`��X�R�A\t\t:" << heightError << endl;
		cerr << "���Έʒu\t\t:" << triggerHeight << endl << endl;

		cerr << "���ז��u���b�N��\t:" << obstacle << endl << endl;

		cerr << "�z��A���X�R�A\t\t:" << maxScore << endl;
		cerr << "���ΘA���X�R�A\t\t:" << attackScore << endl << endl;

		cerr << "�A�����X�R�A\t\t:" << trashScore << endl << endl;

		//cerr << "�����X�R�A\t\t:" << score << endl;
		cerr << waitPoint << ", wait:" << waitNumber << endl;
	}

private:

	//�X�e�[�W��̂��ז��u���b�N�̍����̑��a
	int obstacleScore = 0;
	//10�����Ōq�������u���b�N��
	int linkScore = 0;

	//�u���b�N�̎R�J�̐�
	int flatScore = 0;
	//�u���b�N�̍����̕���
	int average = 0;
	//�u���b�N�̌`��
	int heightError = 0;

	//���Γ_�̍���
	int triggerHeight = 0;

	//���ז��u���b�N�̐�
	int obstacle = 0;

	//�g�܂�Ă���A���̃X�R�A
	int maxScore = 0;
	//���΂������̃X�R�A
	int attackScore = 0;

	//���݂̐�
	int trashScore = 0;

	//���΂���u���b�N
	int waitNumber = 0;
	//���΂�����W
	Point waitPoint;

	//�l�݂��������u���b�N�̐�
	array<int, StageWidth> blockTop;

	//�u���b�N�̓V�ӂ̌���
	//�u������v�̍����𓾂�
	void searchBlockTop(const StageArray& stage) {

		blockTop.fill(0);

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					blockTop[x] = stage.getHeight() - y - 1;
					break;
				}
			}
		}

		average = 0;
		for (const auto& v : blockTop) average += v;
		average /= (int)blockTop.size();
	}
	//���ז��u���b�N�̍����̑��a�𓾂�
	void searchObstacleBlock(const StageArray& stage) {

		obstacleScore = 0;

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == ObstacleBlock)
					obstacleScore += stage.getHeight() - y;//������̍���
				else if (stage[y][x] == EmptyBlock)
					break;
			}
		}
	}
	//10�����Ōq�������u���b�N���𓾂�
	void searchLink(const StageArray& stage) {

		linkScore = 0;

		//�E, ����, ��, �E��
		const Point direction[] = { Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		for (int y = PackSize; y < stage.getHeight(); y++)
		{
			for (int x = 0; x < stage.getWidth(); x++)
			{
				//�u���b�N��10�ȉ��̘A���̌���
				for (const auto& dire : direction)
				{
					int add = stage[y][x];
					Point p = Point(x, y);

					//�T��
					while (true)
					{
						p += dire;

						if (!inside(p) || stage[p] == EmptyBlock) break;

						add += stage[p];

						if (add < AddScore)
							linkScore++;
						else
							break;
					}
				}
			}
		}

	}

	//�u���b�N�̉��ʂ�]������
	//1�u���b�N�̎R�J�̐��𓾂�
	void evaluationFlat(const StageArray& stage) {

		flatScore = 0;

		const int sub = 4;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : StageHeight);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : StageHeight);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				flatScore++;

			if (lsub <= -sub && rsub <= -sub)
				flatScore++;

		}
	}
	//�`��U���^�Ɏ��Ă��邩��]������
	void evaluationShape(const StageArray& stage) {

		heightError = 0;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int h = ((x - StageWidth / 2)*(x - StageWidth / 2) / 5) + (average - 2);
			const int sub = h - blockTop[x];
			heightError += sub*sub;
		}

		heightError = int(sqrt(heightError));

	}

	//�ő�A�����𓾂�
	//���������C���]���ɂȂ�
	void searchChain(const StageArray& stage) {

		maxScore = 0;
		triggerHeight = 0;

		Simulator simulator;

		//����E�E��E���E�E�E�����E���E�E��
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		//�A���̌���
		StageArray maxStage;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			//for (int y = 0; y < PackSize; y++)
			for (int y = 0; y < PackSize; y++)
			{
				const Point point = Point(x, stage.getHeight() - blockTop[x] - 1 - y);
				for (const auto& dire : direction)
				{
					Point p = point;
					int num = AddScore;

					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;
						if (!inside(p) || stage[p] == EmptyBlock) break;

						num -= stage[p];
						if (num <= 0) break;

						int s;
						StageArray checkStage = stage;
						checkStage[point] = num;
						simulator.next(checkStage, s);
						if (maxScore < s)
						{
							maxScore = s;
							triggerHeight = abs(blockTop[x] + y - average);
							maxStage = move(checkStage);
							waitNumber = num;
							waitPoint = point;
						}
					}

				}
			}
		}

	}

};
