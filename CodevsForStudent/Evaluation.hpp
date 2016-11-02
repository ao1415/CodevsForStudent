#pragma once

#include "Simulator.hpp"

class Evaluation {
public:

	Evaluation() = default;
	Evaluation(const StageArray& stage, const int score, const int obstacle, const int turn) {

		setTopBlock(stage);
		evaluationBlockFlat(stage);
		searchChain(stage, score, obstacle, turn);

		totalScore += chainNumber[0] * 100 + chainScore[0];
		totalScore += chainNumber[1] * 10 + chainScore[1] / 10;

		totalScore -= blockFlatScore * 1000;

		//totalScore -= min(chainNumberTriggerRange, chainScoreTriggerRange)*(chainNumber / 10) * 1000;
		//totalScore -= (chainNumberTrigger + chainScoreTrigger) * 10;

	}

	const bool operator<(const Evaluation& e) const { return totalScore < e.totalScore; }

	void show() const {
		cerr << "連鎖:" << tchain << ",スコア:" << tscore << endl;
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

	int tchain = 0;
	int tscore = 0;
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

	void searchChain(const StageArray& stage, const int score, const int obstacle, const int turn) {

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

		const auto trggerCheck = [](const StageArray& stage, const Pack& pack, const Point& triggerPoint, const int triggerNumber) {
			int obstacle = 10;
			const auto obPack = pack.getFullObstacle(obstacle);

			for (int r = 0; r < Rotation; r++)
			{
				const auto arr = obPack.getArray(r);

				for (int x = 0; x < PackSize; x++)
				{
					bool obsFlag = false;
					for (int y = PackSize - 1; y >= 0; y--)
					{
						if (arr[y][x] == ObstacleBlock)
							obsFlag = true;
						if (arr[y][x] == triggerNumber)
						{
							if (!obsFlag)
							{
								if (0 <= triggerPoint.x - x && triggerPoint.x + (PackSize - x - 1) < StageWidth)
								{
									return true;
								}
							}
						}
					}
				}
			}

			return false;
		};

		const auto find_blockTurn = [](const int num, const array<vector<int>, 10>& blockContainPacks, const int turn) {
			for (const auto& val : blockContainPacks[num - 1])
				if (val > turn)
					return val;
			return 1000;
		};

		const auto nearEval = [](const int range) {
			const double e = exp(range - 10);
			const double r = 1 / (1 + e);
			return r;
		};

		Simulator simulator;

		const auto packs = Share::getPacks();
		const auto blockContainPacks = Share::getBlockContainPacks();

		//一度発火させて、発火後の形をもう一度評価してみる？
		//発火ブロックがお邪魔で埋まっても発火できるかみてみる？

		for (int n = 1; n < AddScore; n++)
		{
			int first = find_blockTurn(n, blockContainPacks, turn) - turn;

			for (int x = 0; x < stage.getWidth(); x++)
			{
				const Point point(x, blockTop[x]);

				if (first + turn < Share::getTurn() && trggerCheck(stage, packs[first + turn], point, n))
				{
					if (deleteCheck(point, stage, n))
					{
						auto next = stage;

						next[point] = n;
						int score;
						int chain = simulator.next(next, score);

						const double e = nearEval(first);

						const int escore = int(e*score);
						const int echain = int(e*chain);

						//*
						if (escore > chainScore[0])
						{
							tscore = score;
							chainScore[1] = chainScore[0];
							chainScore[0] = escore;
						}
						else if (escore > chainScore[1])
						{
							chainScore[1] = escore;
						}
						if (echain > chainNumber[0])
						{
							tchain = chain;
							chainNumber[1] = chainNumber[0];
							chainNumber[0] = echain;
						}
						else if (echain > chainNumber[1])
						{
							chainNumber[1] = echain;
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
	}

};
