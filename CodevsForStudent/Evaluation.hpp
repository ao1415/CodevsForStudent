#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn, const int enMaxScore, const int enTriggerTurn, const int enScore) {

		setTopBlock(stage);
		evaluationBlockFlat(stage);
		searchChain(stage, obstacle, turn);

		totalScore += chainNumber[0] * 100 + chainScore[0];
		totalScore += chainNumber[1] * 10 + chainScore[1] / 10;

		totalScore -= blockFlatScore * 1000;

		if (turn != Share::getNow())
		{
			if (score2obstacle(score) >= noneObstacle * 0.5)
			{
				if (score2obstacle(score) >= score2obstacle(enMaxScore))
					totalScore += score * 150;
				if (score2obstacle(score) >= score2obstacle(enScore - 10))
					totalScore += score * 100;
				else
					totalScore -= score * 100;
			}
			totalScore += score;
		}
	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << chainNumber[0] << ",スコア:" << chainScore[0] << endl;
	}

private:

	array<int, StageWidth> blockTop;
	int noneObstacle = 0;

	array<int, 2> chainNumber = {};
	int chainNumberTrigger = StageHeight;
	int chainNumberTriggerRange = INT32_MAX;
	array<int, 2> chainScore = {};
	int chainScoreTrigger = StageHeight;
	int chainScoreTriggerRange = INT32_MAX;

	int blockFlatScore = 0;

	int totalScore = 0;

	void setTopBlock(const StageArray& stage) {

		blockTop.fill(stage.getHeight() - 1);

		for (int x = 0; x < stage.getWidth(); x++)
		{
			for (int y = stage.getHeight() - 1; y >= 0; y--)
			{
				if (stage[y][x] == EmptyBlock)
				{
					blockTop[x] = y;
					noneObstacle += y + 1;
					break;
				}
				if (stage[y][x] != ObstacleBlock)
					noneObstacle++;
			}
		}

	}

	void evaluationBlockFlat(const StageArray& stage) {

		const int sub = 8;

		for (int x = 0; x < (int)blockTop.size(); x++)
		{
			const int l = ((x - 1 >= 0) ? blockTop[x - 1] : 3);
			const int r = ((x + 1 < (int)blockTop.size()) ? blockTop[x + 1] : 3);

			const int lsub = blockTop[x] - l;
			const int rsub = blockTop[x] - r;

			if (lsub >= sub && rsub >= sub)
				blockFlatScore += max(lsub, rsub) - sub + 1;

			if (lsub <= -sub && rsub <= -sub)
				blockFlatScore += -min(lsub, rsub) - sub + 1;

		}
	}

	void searchChain(const StageArray& stage, const int obstacle, const int turn) {

		const auto deleteCheck = [](const Point& pos, const StageArray& stage, const int n) {

			const Point direction[] = { Point(-1,-1),Point(-1,1),Point(-1,0),Point(1,0),Point(1,-1),Point(0,-1),Point(1,-1) };
			for (const auto& dire : direction)
			{
				int add = n;
				Point p = pos;
				while (add < AddScore)
				{
					p += dire;
					if (inside(p) && stage[p] != EmptyBlock) add += stage[p];
					else break;
				}

				if (add == AddScore)
					return true;
			}

			return false;
		};

		const auto find_blockTurn = [](const int num, const array<vector<int>, 10>& blockContainPacks, const int turn) {
			for (const auto& val : blockContainPacks[num - 1])
				if (val > turn)
					return val;
			return 1000;
		};

		const auto nearEval = [](const int range, const int score) {
			const double e = exp(range - 10);
			const double r = 1 / (1 + e);
			return r;
		};

		Simulator simulator;

		const auto blockContainPacks = Share::getBlockContainPacks();

		//一度発火させて、発火後の形をもう一度評価してみる？
		//発火ブロックがお邪魔で埋まっても発火できるかみてみる？

		for (int n = 1; n < AddScore; n++)
		{
			int first = find_blockTurn(n, blockContainPacks, turn) - turn;

			for (int x = 0; x < stage.getWidth(); x++)
			{
				const Point point(x, blockTop[x]);

				if (deleteCheck(point, stage, n))
				{
					auto next = stage;

					next[point] = n;
					int score;
					int chain = simulator.next(next, score);
					const double e = nearEval(first, score);
					score = int(e*score);
					chain = int(e*chain);

					//*
					if (score > chainScore[0])
					{
						chainScore[1] = chainScore[0];
						chainScore[0] = score;
					}
					else if (score > chainScore[1])
					{
						chainScore[1] = score;
					}
					if (chain > chainNumber[0])
					{
						chainNumber[1] = chainNumber[0];
						chainNumber[0] = chain;
					}
					else if (chain > chainNumber[1])
					{
						chainNumber[1] = chain;
					}
					/*/

					//2つぐらい候補出してみる？
					chainScore = max(chainScore, score);
					chainNumber = max(chainNumber, chain);
					*/
				}
			}
		}

	}

};
