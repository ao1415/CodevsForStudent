#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	const int getScore(const StageArray& _stage, const int _obstacleNumber, const int _attackScore, const int _turn) {
		turn = _turn;
		obstacleNumber = _obstacleNumber;
		attackScore = _attackScore;

		searchBlockHeight(_stage);
		addObstacleHeight(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);

		setShotThreshold();

		setTotalScore();

		return totalScore;
	}
	const int getScore(const StageArray& _stage, const int _obstacleNumber, const int _attackScore, const int _turn, const Evaluation& _enemy) {
		turn = _turn;
		obstacleNumber = _obstacleNumber;
		attackScore = _attackScore;

		searchBlockHeight(_stage);
		addObstacleHeight(_stage);
		searchLinkNumber(_stage);

		evaluationBlockFlat(_stage);
		evaluationShapeError(_stage);

		searchChain(_stage);

		setShotThreshold(_enemy);

		setTotalScore();

		return totalScore;
	}

	void show() const {
		cerr << "���ז��z�u�X�R�A\t:" << obstacleHeightSum << endl;
		cerr << "�����X�R�A\t\t:" << blockLink << endl << endl;

		cerr << "���ʃX�R�A\t\t:" << blockFlatScore << endl;
		cerr << "�u���b�N�̍�������\t:" << blockHeightAverage << endl;
		cerr << "U���`��X�R�A\t\t:" << shapeError << endl;
		cerr << "���Έʒu\t\t:" << triggerHeight << endl << endl;

		cerr << "���ז��u���b�N��\t:" << obstacleNumber << endl << endl;

		cerr << "�z��A���X�R�A\t\t:" << chainScore << endl;
		cerr << "���ΘA���X�R�A\t\t:" << attackScore << endl << endl;

		cerr << "�A�����X�R�A\t\t:" << trashNumber << endl << endl;

		cerr << "�����X�R�A\t\t:" << totalScore << endl;
		cerr << triggerPoint << ", wait:" << triggerBlock << endl;
	}

	void showTotalScore() const {
		cerr << "�����X�R�A:" << totalScore << endl;
	}

	void showAttackScore() const {
		cerr << "�`�F�C���X�R�A:" << attackScore << endl;
	}

	const int getAttackScore() const { return attackScore; }

private:

	//���݂̃^�[����
	int turn = 0;

	//�X�e�[�W��̂��ז��u���b�N�̍����̑��a
	int obstacleHeightSum = 0;
	//10�����Ōq�������u���b�N��
	int blockLink = 0;

	//�u���b�N�̎R�J�̐�
	int blockFlatScore = 0;
	//�u���b�N�̍����̕���
	int blockHeightAverage = 0;
	//�u���b�N�̌`��덷
	int shapeError = 0;

	//���Γ_�̍���
	int triggerHeight = 0;

	//���ז��u���b�N�̐�
	int obstacleNumber = 0;

	//�g�܂�Ă���A���̃X�R�A
	int chainScore = 0;
	//���΂������̃X�R�A
	int attackScore = 0;

	//���΂���臒l
	int shotThreshold = 0;

	//���݂̐�
	int trashNumber = 0;

	//�����X�R�A
	int totalScore = 0;
	//���΂���u���b�N
	int triggerBlock = 0;
	//���΂�����W
	Point triggerPoint;

	//�l�݂��������u���b�N�̐�
	array<int, StageWidth> blockTop;

	//�u���b�N�̓V�ӂ̌���
	//�u������v�̍����𓾂�
	void searchBlockHeight(const StageArray& stage) {

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

		blockHeightAverage = 0;
		for (const auto& v : blockTop) blockHeightAverage += v;
		blockHeightAverage /= (int)blockTop.size();
	}
	//���ז��u���b�N�̍����̑��a�𓾂�
	void addObstacleHeight(const StageArray& stage) {

		obstacleHeightSum = 0;

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == ObstacleBlock)
					obstacleHeightSum += stage.getHeight() - y;//������̍���
				else if (stage[y][x] == EmptyBlock)
					break;
			}
		}
	}
	//10�����Ōq�������u���b�N���𓾂�
	void searchLinkNumber(const StageArray& stage) {

		blockLink = 0;

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
							blockLink++;
						else
							break;
					}
				}
			}
		}

	}

	//�u���b�N�̉��ʂ�]������
	//1�u���b�N�̎R�J�̐��𓾂�
	void evaluationBlockFlat(const StageArray& stage) {

		blockFlatScore = 0;

		const int sub = 4;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : StageHeight);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : StageHeight);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				blockFlatScore += max(lsub, rsub) - sub + 1;

			if (lsub <= -sub && rsub <= -sub)
				blockFlatScore += -min(lsub, rsub) - sub + 1;

		}
	}
	//�`��U���^�Ɏ��Ă��邩��]������
	void evaluationShapeError(const StageArray& stage) {

		shapeError = 0;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int h = ((x - StageWidth / 2)*(x - StageWidth / 2) / 5) + (blockHeightAverage - 2);
			const int sub = h - blockTop[x];
			shapeError += sub*sub;
		}

		shapeError = int(sqrt(shapeError));

	}

	//�ő�A�����𓾂�
	//���������C���]���ɂȂ�
	void searchChain(const StageArray& stage) {

		chainScore = 0;
		triggerHeight = 0;

		Simulator simulator;

		const auto& blockContainPacks = Share::getBlockContainPacks();
		const auto find_blockTurn = [&](const int num) {
			for (const auto& val : blockContainPacks[num])
				if (val > turn) return val;
			return 1000;
		};

		//����E�E��E���E�E�E�����E���E�E��
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

		//�A���̌���
		StageArray maxStage;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
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

						for (int i = 0; i < y; i++)
							checkStage[stage.getHeight() - blockTop[x] - 1 - i][x] = ObstacleBlock;

						checkStage[point] = num;
						simulator.next(checkStage, s);

						const int nblockTurn = find_blockTurn(num);
						//const int turnSub = (nblockTurn - turn - 1) / 4;
						const double turnSub = (nblockTurn - turn - 1) / (s < 50 ? 12.0 : 4.0);

						s = (int)(s*exp(-turnSub));

						if (chainScore < s)
						{
							chainScore = s;
							triggerHeight = abs(blockTop[x] + y - blockHeightAverage);
							maxStage = move(checkStage);
							triggerBlock = num;
							triggerPoint = point;
						}
					}

				}
			}
		}

	}

	void setShotThreshold() {

		const int sendBlock = attackScore / 5;

		shotThreshold = 250;

		if (Share::getEnFreeSpace() < sendBlock)
			shotThreshold = 0;

		if (obstacleNumber > 0 && sendBlock > obstacleNumber)
			shotThreshold = 0;

	}
	void setShotThreshold(const Evaluation& enemy) {

		const auto& blockContainPacks = Share::getBlockContainPacks();
		const auto find_blockTurn = [&](const int num) {
			for (const auto& val : blockContainPacks[num])
				if (val > turn) return val;
			return 1000;
		};

		const int sendBlock = attackScore / 5;

		shotThreshold = 250;

		if (Share::getEnFreeSpace() < sendBlock)
		{
			if (enemy.chainScore < chainScore)
			{
				shotThreshold = 0;
				return;
			}
		}

		if (obstacleNumber > 0 && sendBlock > obstacleNumber)
		{
			shotThreshold = 0;
			return;
		}

		const int nblockTurn = find_blockTurn(enemy.triggerBlock);
		const double turnSub = nblockTurn - turn;

		if (turnSub == 1 && sendBlock > 10)
		{
			shotThreshold = 0;
			return;
		}

	}


	void setTotalScore() {

		totalScore -= obstacleHeightSum;
		totalScore += blockLink;

		totalScore -= blockFlatScore * 1000;
		totalScore -= shapeError * 5;
		totalScore -= triggerHeight * 100;

		totalScore -= obstacleNumber * 10;

		totalScore += chainScore * 10;

		//����Ǝ����̏󋵂��݂�臒l�����߂�

		totalScore += attackScore < shotThreshold ? attackScore * 10 : attackScore * 20;

		//totalScore -= chainScore < 250 ? 0 : trashNumber * 3;

	}

};
