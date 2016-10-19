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

								top.score += eval(top, score);

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
			cerr << "score:" << qData.top().score << endl;
			return qData.top().command;
		}

		cerr << "詰みです" << endl;
		return com;
	}

private:

	struct Data {
		StageArray stage;
		int obstacle;
		int score;
		string command;

		const bool operator<(const Data& d) const { return score < d.score; }
	};

	const int eval(const Data& data, const int attackScore) const {

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

		int average =0;
		for (const auto& v : blockTop) average += v;
		average /= blockTop.size();

		//山谷の検索・形の評価
		int flatScore = 0;
		int heightError = 0;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			int l = ((x - 1 >= 0) ? blockTop[x - 1] : StageHeight);
			int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : StageHeight);

			if (blockTop[x] - l >= 4 && blockTop[x] - r >= 4)
				flatScore -= 1000;
			else if (blockTop[x] - l <= -4 && blockTop[x] - r <= -4)
				flatScore -= 1000;

			const int h = (x - 5)*(x - 5) / 5 + average;
			const int sub = h - blockTop[x];
			heightError += sub;
		}
		heightError = int(sqrt(heightError));

		Simulator simulator;

		//左上・右上・左・右・左下・下・右下
		const Point direction[] = { Point(-1,-1),Point(1,-1),Point(-1,0),Point(1,0),Point(-1,1),Point(0,1),Point(1,1) };
		//連鎖の検索
		int maxScore = 0;
		for (int x = 0; x < (int)blockTop.size(); x++)
		{
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
						maxScore = max(maxScore, s);
					}

				}
			}
		}

		int score = 0;

		//cerr << "link:" << linkScore << endl;
		//cerr << "flat:" << flatScore << endl;
		//cerr << "max :" << maxScore << endl;

		score += linkScore;
		score -= obstacleScore;

		score += flatScore;
		score -= heightError;

		score += attackScore < 250 ? attackScore * 10 : attackScore * 20;

		score += maxScore * 10;

		return score;
	}

};
