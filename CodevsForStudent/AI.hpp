#pragma once

#include "Simulator.hpp"

class AI {
public:

	const string think() {
		string com = "0 0";

		const auto packs = Share::getPacks();
		Simulator simulator;

		Data now;
		now.stage = Share::getMyStage();
		now.obstacle = Share::getMyObstacle();
		now.command.clear();
		now.score = 0;

		priority_queue<Data> qData;
		qData.emplace(now);

		const int Turn = 10;
		const int BeamWidth = 10;

		for (int t = 0; t < Turn; t++)
		{
			const int turn = Share::getNow() + t;
			if (turn >= Share::getTurn()) break;

			priority_queue<Data> qNext;

			set<size_t> hashSet;

			for (int i = 0; i < BeamWidth; i++)
			{
				if (qData.empty()) break;

				int obstacle = qData.top().obstacle;

				const auto pack = packs[turn].getFullObstacle(obstacle);
				const auto packArr = pack.getArray();
				const auto sides = pack.getSide();

				for (int r = 0; r < Rotation; r++)
				{
					const int packWidth = sides[r].second - sides[r].first + 1;
					const int left = 0 - sides[r].first;
					const int right = StageWidth - packWidth + 1 - sides[r].first;

					for (int pos = left; pos < right; pos++)
					{
						Data top = qData.top();
						simulator.setBlocks(top.stage, packArr[r], pos);
						simulator.fall(top.stage);

						int score;
						simulator.next(top.stage, score);

						if (!simulator.isDead(top.stage))
						{
							const size_t hash = Hash::FNVa((char*)top.stage.data(), sizeof(StageArray));
							if (hashSet.find(hash) == hashSet.end())
							{
								hashSet.insert(hash);

								ScoreBoard scoreBoard;
								top.score += eval(top, score, scoreBoard);
								//top.score = eval(top, score, scoreBoard);
								top.scoreBoard.emplace_back(scoreBoard);

								top.obstacle = obstacle;
								if (top.command.empty()) top.command = toCommand(pos, r);

								qNext.emplace(top);
							}
						}

					}
				}

				qData.pop();
			}
			qData.swap(qNext);
		}

		if (!qData.empty())
		{
			qData.top().scoreBoard[0].show();
			//cerr << "�����X�R�A\t\t:" << qData.top().score << endl;
			return qData.top().command;
		}

		cerr << "�l�݂ł�" << endl;
		return com;
	}

private:

	struct ScoreBoard {
		int obstacleScore = 0;
		int linkScore = 0;

		int flatScore = 0;
		int average = 0;
		int heightError = 0;
		int triggerHeight = 0;

		int obstacle = 0;

		int maxScore = 0;
		int attackScore = 0;
		int trashScore = 0;

		int score = 0;

		int waitNumber = 0;
		Point point;

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
			cerr << "�����X�R�A\t\t:" << score << endl;
			cerr << point << ", wait:" << waitNumber << endl;
		}

	};

	struct Data {
		StageArray stage;
		int obstacle;
		int score;
		string command;

		vector<ScoreBoard> scoreBoard;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	const int eval(const Data& data, const int attackScore, ScoreBoard& scoreBoard) const {

		array<int, StageWidth> blockTop;

		int linkScore = 0;
		int obstacleScore = 0;
		for (int x = 0; x < data.stage.getWidth(); x++)
		{
			for (int y = data.stage.getHeight() - 1; y >= 0; y--)
			{
				//�u���b�N�̓V�ӂ̌���
				if (data.stage[y][x] == EmptyBlock)
				{
					blockTop[x] = data.stage.getHeight() - y - 1;
					break;
				}
				//���ז��u���b�N�̌���
				else if (data.stage[y][x] == ObstacleBlock)
				{
					obstacleScore += data.stage.getHeight() - y;
				}

				//�E, ����, ��, �E��
				const Point direction[] = { Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

				//�u���b�N��10�ȉ��̘A���̌���
				for (const auto& dire : direction)
				{
					int add = data.stage[y][x];
					Point p = Point(x, y);

					//�T��
					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;

						if (!inside(p) || data.stage[p] == 0) break;

						add += data.stage[p];

						if (add < AddScore)
						{
							linkScore += 5;
						}
						else
							break;
					}
				}
			}
		}

		int average = 0;
		for (const auto& v : blockTop) average += v;
		average /= blockTop.size();

		//�R�J�̌����E�`�̕]��
		int flatScore = 0;
		int heightError = 0;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : 99999);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : 99999);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= 4 && rsub >= 4)
				flatScore -= 1000;

			if (lsub <= -4 && rsub <= -4)
				flatScore -= 1000;

			const int h = (x - 5)*(x - 5) / 5 + average - 2;
			const int sub = h - blockTop[x];
			heightError += sub*sub;
		}
		heightError = int(sqrt(heightError));

		Simulator simulator;

		//����E�E��E���E�E�E�����E���E�E��
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };
		//�A���̌���
		int maxScore = 0;
		int triggerHeight = 0;
		StageArray maxStage;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			//for (int y = 0; y < PackSize; y++)
			for (int y = 0; y < PackSize; y++)
			{
				const Point point = Point(x, data.stage.getHeight() - blockTop[x] - 1 - y);
				for (const auto& dire : direction)
				{
					Point p = point;
					int num = AddScore;

					for (int i = 1; i < StageHeight; i++)
					{
						p += dire;
						if (!inside(p) || data.stage[p] == EmptyBlock) break;

						num -= data.stage[p];
						if (num <= 0) break;

						int s;
						StageArray checkStage = data.stage;
						checkStage[point] = num;
						simulator.next(checkStage, s);
						if (maxScore < s)
						{
							maxScore = s;
							triggerHeight = abs(blockTop[x] + y - average);
							maxStage = move(checkStage);
							scoreBoard.waitNumber = num;
							scoreBoard.point = point;
						}
					}

				}
			}
		}
		int trashScore = 0;
		for (int y = 0; y < maxStage.getHeight(); y++)
		{
			for (int x = 0; x < maxStage.getWidth(); x++)
			{
				if (maxStage[y][x] != EmptyBlock && maxStage[y][x] != ObstacleBlock)
					trashScore++;
			}
		}

		int score = 0;

		score -= obstacleScore;
		score += linkScore;

		score += flatScore;
		score -= heightError * 5;
		score -= triggerHeight * 100;

		score -= data.obstacle * 10;

		score += maxScore * 10;
		//����Ǝ����̏󋵂��݂�臒l�����߂�
		score += attackScore < 250 ? attackScore * 10 : attackScore * 20;

		score -= maxScore < 250 ? 0 : trashScore * 3;

		scoreBoard.obstacleScore = obstacleScore;
		scoreBoard.linkScore = linkScore;

		scoreBoard.flatScore = flatScore;
		scoreBoard.average = average;
		scoreBoard.heightError = heightError;
		scoreBoard.triggerHeight = triggerHeight;

		scoreBoard.obstacle = data.obstacle;

		scoreBoard.attackScore = attackScore;
		scoreBoard.maxScore = maxScore;

		scoreBoard.trashScore = trashScore;

		scoreBoard.score = score;

		return score;
	}

};
