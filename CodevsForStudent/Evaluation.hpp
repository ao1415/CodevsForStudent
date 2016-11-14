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

		if (score2obstacle(score) >= Share::getEnFreeSpace() * 0.5)
		{
			//if (score > enMaxScore)
			//	totalScore += score * 100;
			if (score2obstacle(score) >= score2obstacle(enScore - 10))
				totalScore += score * 100;
			else
				totalScore -= score * 100;
		}
		totalScore += score;

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << chainNumber[0] << ",スコア:" << chainScore[0] << endl;
	}

private:

	array<int, StageWidth> blockTop;

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
					break;
				}
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

			for (int xy = 0; xy < StageWidth; xy++)
			{
				const int m = min(pos.x, pos.y);
				int px = pos.x - m + xy;
				int py = pos.y - m + xy;

				if (!(px < StageWidth && py < StageHeight + 3)) break;
				int add = stage[py][px];
				if (add == EmptyBlock) continue;

				int d = 1;
				while (add < AddScore)
				{
					if (px + d < StageWidth && py + d < StageHeight + 3)
					{
						if (stage[py + d][px + d] != EmptyBlock) add += stage[py + d][px + d];
						else break;
					}
					else break;
					d++;
				}

				if (add == AddScore)
					return true;
			}

			for (int xy = 0; xy < StageWidth; xy++)
			{
				const int m = min(pos.x, StageHeight + 2 - pos.y);
				int px = pos.x - m + xy;
				int py = pos.y + StageHeight + 2 - m - xy;

				if (!(px < StageWidth && py >= 0)) break;
				int add = stage[py][px];
				if (add == EmptyBlock) continue;

				int d = 1;
				while (add < AddScore)
				{
					if (px + d < StageWidth && py - d >= 0)
					{
						if (stage[py - d][px + d] != EmptyBlock) add += stage[py - d][px + d];
						else break;
					}
					else break;
					d++;
				}

				if (add == AddScore)
					return true;
			}

			for (int x = 0; x < StageWidth; x++)
			{
				int add = stage[pos.y][x];
				if (add == EmptyBlock) continue;

				int dx = 1;
				while (add < AddScore)
				{
					if (x + dx < StageWidth && stage[pos.y][x + dx] != EmptyBlock) add += stage[pos.y][x + dx];
					else break;
					dx++;
				}

				if (add == AddScore)
					return true;
			}

			for (int y = 0; y < StageHeight + 3; y++)
			{
				int add = stage[y][pos.x];
				if (add == EmptyBlock) continue;

				int dy = 1;
				while (add < AddScore)
				{
					if (y + dy < StageHeight + 3 && stage[y + dy][pos.x] != EmptyBlock) add += stage[y + dy][pos.x];
					else break;
					dy++;
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

				auto next = stage;
				next[point] = n;

				if (deleteCheck(point, next, n))
				{
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
