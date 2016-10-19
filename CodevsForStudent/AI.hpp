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
			//cerr << "総合スコア\t\t:" << qData.top().score << endl;
			return qData.top().command;
		}

		cerr << "詰みです" << endl;
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
			cerr << "お邪魔配置スコア\t:" << obstacleScore << endl;
			cerr << "結合スコア\t\t:" << linkScore << endl << endl;

			cerr << "凹凸スコア\t\t:" << flatScore << endl;
			cerr << "ブロックの高さ平均\t:" << average << endl;
			cerr << "U字形状スコア\t\t:" << heightError << endl;
			cerr << "発火位置\t\t:" << triggerHeight << endl << endl;

			cerr << "お邪魔ブロック数\t:" << obstacle << endl << endl;

			cerr << "想定連鎖スコア\t\t:" << maxScore << endl;
			cerr << "発火連鎖スコア\t\t:" << attackScore << endl << endl;
			cerr << "連鎖尾スコア\t\t:" << trashScore << endl << endl;
			cerr << "総合スコア\t\t:" << score << endl;
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
				//ブロックの天辺の検索
				if (data.stage[y][x] == EmptyBlock)
				{
					blockTop[x] = data.stage.getHeight() - y - 1;
					break;
				}
				//お邪魔ブロックの検索
				else if (data.stage[y][x] == ObstacleBlock)
				{
					obstacleScore += data.stage.getHeight() - y;
				}

				//右, 左下, 下, 右下
				const Point direction[] = { Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };

				//ブロックの10以下の連結の検索
				for (const auto& dire : direction)
				{
					int add = data.stage[y][x];
					Point p = Point(x, y);

					//探査
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

		//山谷の検索・形の評価
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

		//左上・右上・左・右・左下・下・右下
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };
		//連鎖の検索
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
		//相手と自分の状況をみて閾値を決める
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
